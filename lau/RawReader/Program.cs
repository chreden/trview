using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RawReader
{
    class Program
    {
        static void ConvertImage(string path)
        {
            using var binary = new BinaryReader(new FileStream(path, FileMode.Open));

            // !WAR
            binary.BaseStream.Seek(4, SeekOrigin.Current);
            // More unknowns
            binary.BaseStream.Seek(16, SeekOrigin.Current);
            var width = binary.ReadInt32();
            var height = binary.ReadInt32();
            // ????
            binary.BaseStream.Seek(100, SeekOrigin.Current);

            using var image = new Bitmap(width, height);

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    var pixel = binary.ReadInt32();
                    image.SetPixel(x, y, Color.FromArgb(pixel));
                }
            }

            image.Save(Path.Combine("images", Path.GetFileNameWithoutExtension(path) + ".png"), ImageFormat.Png);
        }

        static void Main(string[] args)
        {
            var path = @"D:\Games\Steam\steamapps\common\Tomb Raider Anniversary\bigfile_unpack\default\__UNKNOWN\raw";
            // var path = @"D:\Games\Steam\steamapps\common\Tomb Raider Anniversary\bigfile_unpack\default\pc-w";
            foreach (var file in Directory.GetFiles(path, "*.raw"))
            {
                ConvertImage(file);
            }
        }
    }
}
