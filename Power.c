#include "Driver.h"
#include "Power.tmh"


_Use_decl_annotations_
NTSTATUS
DsHidMini_EvtWdfDeviceSelfManagedIoInit(
	WDFDEVICE  Device
)
{
	NTSTATUS				status = STATUS_SUCCESS;
	PDEVICE_CONTEXT			pDeviceContext;
	WDF_OBJECT_ATTRIBUTES	attribs;

	UNREFERENCED_PARAMETER(Device);

	PAGED_CODE();

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_POWER, "%!FUNC! Entry");

	pDeviceContext = DeviceGetContext(Device);

	if (pDeviceContext->ConnectionType == DsHidMiniDeviceConnectionTypeBth)
	{
		WDF_OBJECT_ATTRIBUTES_INIT(&attribs);
		attribs.ParentObject = Device;

		status = WdfRequestCreate(
			&attribs,
			pDeviceContext->Connection.Bth.BthIoTarget,
			&pDeviceContext->Connection.Bth.HidInterruptReadRequest
		);
		if (!NT_SUCCESS(status))
		{
			TraceEvents(TRACE_LEVEL_ERROR,
				TRACE_POWER,
				"WdfRequestCreate failed with status %!STATUS!",
				status
			);
		}

		status = WdfMemoryCreate(
			&attribs,
			NonPagedPool,
			DS3_POOL_TAG,
			BTHPS3_SIXAXIS_HID_INPUT_REPORT_SIZE,
			&pDeviceContext->Connection.Bth.HidInterruptReadMemory,
			NULL
		);
		if (!NT_SUCCESS(status))
		{
			TraceEvents(TRACE_LEVEL_ERROR,
				TRACE_POWER,
				"WdfMemoryCreate failed with status %!STATUS!",
				status
			);
		}

		DsBth_Ds3Init(pDeviceContext);

		status = WdfIoTargetFormatRequestForIoctl(
			pDeviceContext->Connection.Bth.BthIoTarget,
			pDeviceContext->Connection.Bth.HidInterruptReadRequest,
			IOCTL_BTHPS3_HID_INTERRUPT_READ,
			NULL,
			NULL,
			pDeviceContext->Connection.Bth.HidInterruptReadMemory,
			NULL
		);
		if (!NT_SUCCESS(status))
		{
			TraceEvents(TRACE_LEVEL_ERROR,
				TRACE_POWER,
				"WdfIoTargetFormatRequestForIoctl failed with status %!STATUS!",
				status
			);
		}

		WdfRequestSetCompletionRoutine(
			pDeviceContext->Connection.Bth.HidInterruptReadRequest,
			DsBth_HidInterruptReadRequestCompletionRoutine,
			pDeviceContext
		);

		if (WdfRequestSend(
			pDeviceContext->Connection.Bth.HidInterruptReadRequest,
			pDeviceContext->Connection.Bth.BthIoTarget,
			WDF_NO_SEND_OPTIONS
		) == FALSE) {
			status = WdfRequestGetStatus(pDeviceContext->Connection.Bth.HidInterruptReadRequest);
		}
		if (!NT_SUCCESS(status))
		{
			TraceEvents(TRACE_LEVEL_ERROR,
				TRACE_POWER,
				"WdfRequestSend failed with status %!STATUS!",
				status
			);
		}
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_POWER, "%!FUNC! Exit");

	return status;
}

_Use_decl_annotations_
VOID
DsHidMini_EvtWdfDeviceSelfManagedIoCleanup(
	WDFDEVICE  Device
)
{
	UNREFERENCED_PARAMETER(Device);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_POWER, "%!FUNC! Entry");

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_POWER, "%!FUNC! Exit");
}

_Use_decl_annotations_
NTSTATUS
DsHidMini_EvtDevicePrepareHardware(
	WDFDEVICE  Device,
	WDFCMRESLIST  ResourcesRaw,
	WDFCMRESLIST  ResourcesTranslated
)
{
	NTSTATUS                                status = STATUS_SUCCESS;
	PDEVICE_CONTEXT                         pDeviceContext;
	WDF_USB_DEVICE_SELECT_CONFIG_PARAMS     configParams;
	UCHAR                                   index;
	WDFUSBPIPE                              pipe;
	WDF_USB_PIPE_INFORMATION                pipeInfo;

	UNREFERENCED_PARAMETER(ResourcesRaw);
	UNREFERENCED_PARAMETER(ResourcesTranslated);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_POWER, "%!FUNC! Entry");

	pDeviceContext = DeviceGetContext(Device);

	if (pDeviceContext->ConnectionType == DsHidMiniDeviceConnectionTypeUnknown
		&& pDeviceContext->Connection.Usb.UsbDevice == NULL)
	{
		status = WdfUsbTargetDeviceCreate(Device,
			WDF_NO_OBJECT_ATTRIBUTES,
			&pDeviceContext->Connection.Usb.UsbDevice
		);

		if (NT_SUCCESS(status))
		{
			pDeviceContext->ConnectionType = DsHidMiniDeviceConnectionTypeUsb;
			goto prepareInit;
		}

		pDeviceContext->ConnectionType = DsHidMiniDeviceConnectionTypeBth;
		pDeviceContext->Connection.Bth.BthIoTarget = WdfDeviceGetIoTarget(Device);
		status = STATUS_SUCCESS;
	}

prepareInit:

	if (pDeviceContext->ConnectionType == DsHidMiniDeviceConnectionTypeUsb)
	{
#pragma region USB Interface & Pipe settings

		WDF_USB_DEVICE_SELECT_CONFIG_PARAMS_INIT_SINGLE_INTERFACE(&configParams);

		status = WdfUsbTargetDeviceSelectConfig(pDeviceContext->Connection.Usb.UsbDevice,
			WDF_NO_OBJECT_ATTRIBUTES,
			&configParams
		);

		if (!NT_SUCCESS(status)) {
			TraceEvents(TRACE_LEVEL_ERROR, TRACE_POWER,
				"WdfUsbTargetDeviceSelectConfig failed %!STATUS!", status);
			return status;
		}

		pDeviceContext->Connection.Usb.UsbInterface = configParams.Types.SingleInterface.ConfiguredUsbInterface;

		//
		// Get pipe handles
		//
		for (index = 0; index < WdfUsbInterfaceGetNumConfiguredPipes(pDeviceContext->Connection.Usb.UsbInterface); index++) {

			WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);

			pipe = WdfUsbInterfaceGetConfiguredPipe(
				pDeviceContext->Connection.Usb.UsbInterface,
				index, //PipeIndex,
				&pipeInfo
			);
			//
			// Tell the framework that it's okay to read less than
			// MaximumPacketSize
			//
			WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(pipe);

			if (WdfUsbPipeTypeInterrupt == pipeInfo.PipeType &&
				WdfUsbTargetPipeIsInEndpoint(pipe)) {
				TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_POWER,
					"InterruptReadPipe is 0x%p\n", pipe);
				pDeviceContext->Connection.Usb.InterruptInPipe = pipe;
			}

			if (WdfUsbPipeTypeInterrupt == pipeInfo.PipeType &&
				WdfUsbTargetPipeIsOutEndpoint(pipe)) {
				TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_POWER,
					"InterruptWritePipe is 0x%p\n", pipe);
				pDeviceContext->Connection.Usb.InterruptOutPipe = pipe;
			}
		}

		if (!pDeviceContext->Connection.Usb.InterruptInPipe || !pDeviceContext->Connection.Usb.InterruptOutPipe)
		{
			status = STATUS_INVALID_DEVICE_STATE;
			TraceEvents(TRACE_LEVEL_ERROR, TRACE_POWER,
				"Device is not configured properly %!STATUS!\n",
				status);

			return status;
		}

#pragma endregion

		status = DsUsbConfigContReaderForInterruptEndPoint(Device);
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_POWER, "%!FUNC! Exit (%!STATUS!)", status);

	return status;
}

NTSTATUS DsHidMini_EvtDeviceD0Entry(
	_In_ WDFDEVICE              Device,
	_In_ WDF_POWER_DEVICE_STATE PreviousState
)
{
	PDEVICE_CONTEXT         pDeviceContext;
	NTSTATUS                status = STATUS_SUCCESS;
	BOOLEAN                 isTargetStarted;

	pDeviceContext = DeviceGetContext(Device);
	isTargetStarted = FALSE;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_POWER, "%!FUNC! Entry");

	UNREFERENCED_PARAMETER(PreviousState);

	if (pDeviceContext->ConnectionType == DsHidMiniDeviceConnectionTypeUsb)
	{
		//
		// Since continuous reader is configured for this interrupt-pipe, we must explicitly start
		// the I/O target to get the framework to post read requests.
		//
		status = WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(pDeviceContext->Connection.Usb.InterruptInPipe));
		if (!NT_SUCCESS(status)) {
			TraceEvents(TRACE_LEVEL_ERROR, TRACE_POWER, "Failed to start interrupt read pipe %!STATUS!\n", status);
			goto End;
		}

		status = WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(pDeviceContext->Connection.Usb.InterruptOutPipe));
		if (!NT_SUCCESS(status)) {
			TraceEvents(TRACE_LEVEL_ERROR, TRACE_POWER, "Failed to start interrupt write pipe %!STATUS!\n", status);
			goto End;
		}

		isTargetStarted = TRUE;

	End:

		if (!NT_SUCCESS(status)) {
			//
			// Failure in D0Entry will lead to device being removed. So let us stop the continuous
			// reader in preparation for the ensuing remove.
			//
			if (isTargetStarted) {
				WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(pDeviceContext->Connection.Usb.InterruptInPipe), WdfIoTargetCancelSentIo);
				WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(pDeviceContext->Connection.Usb.InterruptOutPipe), WdfIoTargetCancelSentIo);
			}
		}

		status = DsUsb_Ds3Init(pDeviceContext);

		if (!NT_SUCCESS(status))
		{
			TraceEvents(TRACE_LEVEL_ERROR, TRACE_POWER,
				"DsUsb_Ds3Init failed with status %!STATUS!",
				status);
		}
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_POWER, "%!FUNC! Exit");

	return status;
}

NTSTATUS DsHidMini_EvtDeviceD0Exit(
	_In_ WDFDEVICE              Device,
	_In_ WDF_POWER_DEVICE_STATE TargetState
)
{
	PDEVICE_CONTEXT pDeviceContext;

	UNREFERENCED_PARAMETER(TargetState);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_POWER, "%!FUNC! Entry");

	pDeviceContext = DeviceGetContext(Device);

	if (pDeviceContext->ConnectionType == DsHidMiniDeviceConnectionTypeUsb)
	{
		WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(
			pDeviceContext->Connection.Usb.InterruptInPipe),
			WdfIoTargetCancelSentIo
		);
		WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(
			pDeviceContext->Connection.Usb.InterruptOutPipe),
			WdfIoTargetCancelSentIo
		);
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_POWER, "%!FUNC! Exit");

	return STATUS_SUCCESS;
}
