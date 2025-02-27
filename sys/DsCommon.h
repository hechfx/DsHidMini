#pragma once


//
// Defines a Bluetooth client MAC address
// 
#include <pshpack1.h>
typedef struct _BD_ADDR
{
	UCHAR Address[6];

} BD_ADDR, * PBD_ADDR;
#include <poppack.h>

//
// Device type
// 
typedef enum
{
	//
	// Unknown device type
	// 
	DsDeviceTypeUnknown,

	//
	// Sony DualShock 3 Controller
	// 
	DsDeviceTypeSixaxis,

	//
	// Sony Navigation Controller
	// 
	DsDeviceTypeNavigation,

	//
	// Sony Motion Controller
	// 
	DsDeviceTypeMotion,

	//
	// Sony DualShock 4 Controller
	// 
	DsDeviceTypeWireless

} DS_DEVICE_TYPE, * PDS_DEVICE_TYPE;

//
// Device connection type
// 
typedef enum
{
	DsDeviceConnectionTypeUnknown = 0x00,
	DsDeviceConnectionTypeUsb,
	DsDeviceConnectionTypeBth

} DS_CONNECTION_TYPE, * PDS_CONNECTION_TYPE;

//
// Battery status values reported by the hardware
// 
typedef enum
{
	DsBatteryStatusNone = 0x00,
	DsBatteryStatusDying = 0x01,
	DsBatteryStatusLow = 0x02,
	DsBatteryStatusMedium = 0x03,
	DsBatteryStatusHigh = 0x04,
	DsBatteryStatusFull = 0x05,
	DsBatteryStatusCharging = 0xEE,
	DsBatteryStatusCharged = 0xEF

} DS_BATTERY_STATUS, * PDS_BATTERY_STATUS;

//
// See https://vigem.org/projects/DsHidMini/HID-Device-Modes-Explained
// 
typedef enum
{
	//
	// Unknown/invalid
	// 
	DsHidMiniDeviceModeUnknown = 0,
	//
	// Single device with Force Feedback
	// 
	DsHidMiniDeviceModeSDF,
	//
	// Gamepad plus Joystick with Force Feedback
	// 
	DsHidMiniDeviceModeGPJ,
	//
	// Sony SIXAXIS.SYS compatible
	// 
	DsHidMiniDeviceModeSixaxisCompatible,
	//
	// DS4Windows compatible custom device
	// 
	DsHidMiniDeviceModeDS4WindowsCompatible,
	//
	// Microsoft XINPUTHID.SYS compatible
	// 
	DsHidMiniDeviceModeXInputHIDCompatible

} DS_HID_DEVICE_MODE, * PDS_HID_DEVICE_MODE;

//
// String-representation of the modes ordered by index
// 
static CONST PSTR G_HID_DEVICE_MODE_NAMES[] =
{
	"\0", // Reserved/invalid
	"SDF",
	"GPJ",
	"SXS",
	"DS4Windows",
	"XInput"
};

//
// Output report processing mode
// 
typedef enum
{
	//
	// DSHM controls output report generation
	// 
	Ds3OutputReportModeDriverHandled = 0,

	//
	// Output reports come in from "the outside" and get passed on
	// 
	Ds3OutputReportModeWriteReportPassThrough

} DS_OUTPUT_REPORT_MODE, * PDS_OUTPUT_REPORT_MODE;

//
// The originator of output reports
// 
typedef enum
{
	//
	// Request originated from ourself (must not be discarded)
	// 
	Ds3OutputReportSourceDriverHighPriority = 0,

	//
	// Request originated from ourself (may be discarded)
	// 
	Ds3OutputReportSourceDriverLowPriority,

	//
	// Request came in through Force Feedback exposure
	// 
	Ds3OutputReportSourceForceFeedback,

	//
	// Request came in through SXS exposure
	// 
	Ds3OutputReportSourcePassThrough,

	//
	// Request came in from DS4 emulation
	// 
	Ds3OutputReportSourceDualShock4,

	//
	// Request came from XINPUTHID.SYS
	// 
	Ds3OutputReportSourceXInputHID

} DS_OUTPUT_REPORT_SOURCE, * PDS_OUTPUT_REPORT_SOURCE;

//
// Flags of whether pressure sensitive button variants should be exposed as digital, analogue or both
// 
typedef enum
{
	//
	// Only report digital (two-state) pressure sensitive button state changes
	// 
	DsPressureExposureModeDigital = 1 << 0,
	//
	// Only report analogue (range) pressure sensitive button state changes
	// 
	DsPressureExposureModeAnalogue = 1 << 1,
	//
	// Default behaviour exposes both unaltered
	// 
	DsPressureExposureModeDefault = DsPressureExposureModeDigital | DsPressureExposureModeAnalogue

} DS_PRESSURE_EXPOSURE_MODE, * PDS_PRESSURE_EXPOSURE_MODE;

//
// Friendly names for reading from JSON
// 
static CONST PSTR G_PRESSURE_EXPOSURE_MODE_NAMES[] =
{
	"Digital",
	"Analogue",
	"Default"
};

//
// Flags of whether the D-Pad should be reported as HAT switch or individual buttons
// 
typedef enum
{
	//
	// Expose as default HAT/POV format
	// 
	DsDPadExposureModeHAT = 1 << 0,
	//
	// Expose as individual buttons
	// 
	DsDPadExposureModeIndividualButtons = 1 << 1,
	//
	// Default behaviour exposes HAT/POV format
	// 
	DsDPadExposureModeDefault = DsDPadExposureModeHAT

} DS_DPAD_EXPOSURE_MODE, * PDS_DPAD_EXPOSURE_MODE;

//
// Friendly names for reading from JSON
// 
static CONST PSTR G_DPAD_EXPOSURE_MODE_NAMES[] =
{
	"HAT",
	"IndividualButtons",
	"Default"
};

//
// Possible LED modes
// 
typedef enum
{
	//
	// Invalid
	// 
	DsLEDModeUnknown = 0,
	//
	// Use only one player index to indicate charge (1 = Lowest, 4 = Highest)
	// 
	DsLEDModeBatteryIndicatorPlayerIndex,
	//
	// Display charge as simulated bar graph (1 = Lowest, 1 to 4 = Highest)
	// 
	DsLEDModeBatteryIndicatorBarGraph,
	//
	// Use whatever pattern is provided by configuration
	// 
	DsLEDModeCustomPattern

} DS_LED_MODE;

//
// Friendly names for reading from JSON
// 
static CONST PSTR G_LED_MODE_NAMES[] =
{
	"BatteryIndicatorPlayerIndex",
	"BatteryIndicatorBarGraph",
	"CustomPattern"
};

//
// Whether the driver or the application will have authority over LEDs
// 
typedef enum
{
	//
	// Give driver authority on boot, switch over to app if requested
	// 
	DsLEDAuthorityAutomatic = 0,
	//
	// The driver is always in charge of LED manipulation
	// 
	DsLEDAuthorityDriver,
	//
	// The application is in charge, the driver will do nothing
	// 
	DsLEDAuthorityApplication

} DS_LED_AUTHORITY;

//
// Friendly names for reading from JSON
// 
static CONST PSTR G_DS_LED_AUTHORITY_NAMES[] =
{
	"Automatic",
	"Driver",
	"Application"
};

//
// Axis dead-zone settings
// 
typedef struct _DS_AXIS_DEADZONE
{
	//
	// Should dead-zone be applied
	// 
	BOOLEAN Apply;

	//
	// Dead-zone radius (0-360)
	// 
	DOUBLE PolarValue;

} DS_AXIS_DEADZONE, * PDS_AXIS_DEADZONE;

//
// Thumb settings
// 
typedef struct _DS_THUMB_SETTINGS
{
	//
	// Dead-zone of left thumb stick
	// 
	DS_AXIS_DEADZONE DeadZoneLeft;

	//
	// Dead-zone of right thumb stick
	// 
	DS_AXIS_DEADZONE DeadZoneRight;

} DS_THUMB_SETTINGS, * PDS_THUMB_SETTINGS;

//
// Rumble settings
// 
typedef struct _DS_RUMBLE_SETTINGS
{
	//
	// Disable Big Motor (left) entirely
	// 
	BOOLEAN DisableBM;

	//
	// Disable Small Motor (right) entirely
	// 
	BOOLEAN DisableSM;

	struct
	{
		BOOLEAN Enabled;

		UCHAR MinValue;

		UCHAR MaxValue;

		DOUBLE ConstA;

		DOUBLE ConstB;

	} BMStrRescale;

	struct
	{
		BOOLEAN Enabled;

		UCHAR RescaleMinValue;

		UCHAR RescaleMaxValue;

		DOUBLE ConstA;

		DOUBLE ConstB;

	} SMToBMConversion;

	struct
	{
		BOOLEAN BMThresholdEnabled;

		UCHAR BMThresholdValue;

		BOOLEAN SMThresholdEnabled;

		UCHAR SMThresholdValue;

	} ForcedSM;

} DS_RUMBLE_SETTINGS, * PDS_RUMBLE_SETTINGS;

//
// https://github.com/RPCS3/rpcs3/blob/abd8bd6f3619c805d52807f321b78d4baf89c252/rpcs3/Input/ds3_pad_handler.cpp#L18-L25
// 
typedef struct _DS_LED
{
	UCHAR Duration;

	UCHAR IntervalDuration;

	UCHAR EnabledFlags;

	UCHAR IntervalPortionOff;

	UCHAR IntervalPortionOn;

} DS_LED, * PDS_LED;

//
// LED Customization
// 
typedef struct _DS_LED_SETTINGS
{
	//
	// LED behaviour when driver is in charge
	// 
	DS_LED_MODE Mode;

	//
	// LED update authority (who is allowed to modify LED states)
	// 
	DS_LED_AUTHORITY Authority;

	//
	// Custom LED parameters
	// 
	struct
	{
		UCHAR LEDFlags;

		DS_LED Player1;

		DS_LED Player2;

		DS_LED Player3;

		DS_LED Player4;

	} CustomPatterns;

} DS_LED_SETTINGS, * PDS_LED_SETTINGS;

//
// Flags whether to flip (invert) axes
// 
typedef struct _DS_FLIP_AXIS_SETTINGS
{
	UCHAR LeftX;

	UCHAR LeftY;

	UCHAR RightX;

	UCHAR RightY;
	
} DS_FLIP_AXIS_SETTINGS, *PDS_FLIP_AXIS_SETTINGS;

//
// Per device dynamic configuration properties
// 
typedef struct _DS_DRIVER_CONFIGURATION
{
	//
	// The HID device mode
	// Can't be altered at runtime
	// 
	DS_HID_DEVICE_MODE HidDeviceMode;

	//
	// When set, pairing will not be attempted on device boot
	// Can't be altered at runtime
	// 
	BOOLEAN DisableAutoPairing;

	//
	// True if output rate control is enabled, false if not
	// 
	BOOLEAN IsOutputRateControlEnabled;

	//
	// The output rate control period in milliseconds
	// 
	UCHAR OutputRateControlPeriodMs;

	//
	// True if output deduplicator is enabled, false if not
	// 
	UCHAR IsOutputDeduplicatorEnabled;

	//
	// Idle disconnect period in milliseconds
	// 
	ULONG WirelessIdleTimeoutPeriodMs;

	//
	// If set, controller will never auto-disconnect on wireless
	// 
	BOOLEAN DisableWirelessIdleTimeout;

	//
	// Thumb stick specific settings
	// 
	DS_THUMB_SETTINGS ThumbSettings;

	//
	// Rumble customizing
	// 
	DS_RUMBLE_SETTINGS RumbleSettings;

	//
	// LED customizing
	// 
	DS_LED_SETTINGS LEDSettings;

	//
	// Flip axis settings
	// 
	DS_FLIP_AXIS_SETTINGS FlipAxis;

	//
	// SDF-mode specific
	// 
	struct
	{
		DS_PRESSURE_EXPOSURE_MODE PressureExposureMode;

		DS_DPAD_EXPOSURE_MODE DPadExposureMode;

	} SDF;

	//
	// GPJ-mode specific
	// 
	struct
	{
		DS_PRESSURE_EXPOSURE_MODE PressureExposureMode;

		DS_DPAD_EXPOSURE_MODE DPadExposureMode;

	} GPJ;

} DS_DRIVER_CONFIGURATION, * PDS_DRIVER_CONFIGURATION;
