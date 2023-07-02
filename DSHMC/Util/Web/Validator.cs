using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using Newtonsoft.Json;

namespace Nefarius.DsHidMini.Util.Web
{
    public class OUIEntry : IEquatable<OUIEntry>
    {
        public OUIEntry(string manufacturer)
        {
            var hex = manufacturer.Replace(":", string.Empty);

            Bytes = Enumerable.Range(0, hex.Length / 2).Select(x => Convert.ToByte(hex.Substring(x * 2, 2), 16))
                .ToArray();
        }

        public OUIEntry(PhysicalAddress address)
        {
            Bytes = address.GetAddressBytes().Take(3).ToArray();
        }

        public byte[] Bytes { get; }

        public override bool Equals(object obj)
        {
            if (obj is OUIEntry entry)
                return Bytes.SequenceEqual(entry.Bytes);
            return false;
        }

        public bool Equals(OUIEntry other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return Bytes.SequenceEqual(other.Bytes);
        }

        public override int GetHashCode()
        {
            return Bytes.GetHashCode();
        }
    }

    public class Validator
    {
        public static Uri ApiUrl => new Uri("https://vigem.org/projects/DsHidMini/genuine_oui_db.json");

        var macs = new IList<string> {
                "00:02:C7",
                "00:06:F5",
                "00:06:F7",
                "00:07:04",
                "00:16:FE",
                "00:19:C1",
                "00:1B:FB",
                "00:1E:3D",
                "00:21:4F",
                "00:23:06",
                "00:24:33",
                "00:26:43",
                "04:76:6E",
                "04:98:F3",
                "28:A1:83",
                "30:C3:D9",
                "34:C7:31",
                "38:C0:96",
                "48:F0:7B",
                "58:16:D7",
                "60:38:0E",
                "64:05:E4",
                "64:D4:BD",
                "74:95:EC",
                "9C:8D:7C",
                "AC:7A:4D",
                "B4:EC:02",
                "BC:42:8C",
                "BC:75:36",
                "E0:75:0A",
                "E0:AE:5E",
                "FC:62:B9",
                "00:04:1F",
                "00:13:15",
                "00:15:C1",
                "00:19:C5",
                "00:1D:0D",
                "00:1F:A7",
                "00:24:8D",
                "00:D9:D1",
                "00:E4:21",
                "0C:FE:45",
                "28:0D:FC",
                "2C:CC:44",
                "70:9E:29",
                "78:C8:81",
                "A8:E3:EE",
                "BC:60:A7",
                "C8:63:F1",
                "F8:46:1C",
                "F8:D0:AC",
                "FC:0F:E6",
                "44:D8:32",
                "08:A9:5A",
                "22:AF:62"
        };

        

        public static bool IsGenuineAddress(PhysicalAddress address)
        {
            using (var client = new WebClient())
            {
                var ouiList = macs;

                var device = new OUIEntry(address);

                return ouiList.Contains(device);
            }
        }
    }
}
