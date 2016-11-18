using System;
using System.ComponentModel;

namespace Ogose
{

    /// <summary> 日周緯度変コントローラへ送るコマンド文字列を生成する関数群をまとめたクラス。 </summary>
    public class NisshuidohenController
{
    /// <summary> 日周・緯度のモータのうち一方を選ぶための定数 </summary>
    public enum Motor
    {
        /// <summary> 日周運動用モータ </summary>
        Diurnal,
        /// <summary> 緯度運動用モータ </summary>
        Latitude,
    }

    private string RotateSpeed(string motor, string orientation, int speed)
    {
        return "$W" + "0" + "7V" + motor + orientation + speed.ToString("X4");
    }
    private string RotateAngle(string motor, string orientation, int angle, int speed)
    {
        return "$W" + "0" + "DP" + motor + orientation + angle.ToString("X6") + speed.ToString("X4");
    }

    private static string ToString(Motor motor)
    {
        switch (motor)
        {
            case Motor.Diurnal:
                return "D";
            case Motor.Latitude:
                return "L";
            default:
                throw new InvalidEnumArgumentException("motor", (int)motor, typeof(Motor));
        }
    }
    private static string Orientation(int angle)
    {
        return angle >= 0 ? "+" : "-";
    }

    public string RotateDiurnalBySpeed(int speed)
    {
        return RotateBySpeed(Motor.Diurnal, speed);
    }
    public string RotateLatitudeBySpeed(int speed)
    {
        return RotateBySpeed(Motor.Latitude, speed);
    }
    /// <summary> 指定したモータを一定速度でずっと回転し続ける。 </summary>
    /// <param name="motor">動かすモータ。</param>
    /// <param name="speed">日周なら、毎秒 4/1000°単位。緯度なら、毎秒 9/8192°単位。</param>
    /// <returns>コマンド文字列</returns>
    public string RotateBySpeed(Motor motor, int speed)
    {
        if (speed < -0xFFFF || 0xFFFF < speed)
            return null;
        return RotateSpeed(ToString(motor), Orientation(speed), Math.Abs(speed));
    }

    /// <summary> 日周モータを指定の角度だけ指定した速度で回転させる。このメソッドは使ったことない。 </summary>
    /// <param name="angle">9000000で360°、上限は16777215</param>
    /// <param name="time">不明</param>
    /// <returns></returns>
    public string RotateDiurnalByAngle(int angle, int time)
    {
        return RotateByAngle(Motor.Diurnal, angle, time);
    }
    /// <summary> 緯度モータを指定の角度だけ指定した速度で回転させる。このメソッドは使ったことない。 </summary>
    /// <param name="angle">16384000で180°、上限は167772115</param>
    /// <param name="time">不明</param>
    /// <returns>コマンド文字列</returns>
    public string RotateLatitudeByAngle(int angle, int time)
    {
        return RotateByAngle(Motor.Latitude, angle, time);
    }
    /// <summary> 指定したモータを指定の角度だけ指定した速度で回転させる。このメソッドは使ったことない。 </summary>
    /// <param name="motor"></param>
    /// <param name="angle">日周なら、4/100,000°単位。緯度なら、1.0986328125/100000°単位。</param>
    /// <param name="time">不明</param>
    /// <returns>コマンド文字列</returns>
    public string RotateByAngle(Motor motor, int angle, int time)
    {
        if (angle < -0xFFFFFF || 0xFFFFFF < angle || time <= 0) // 16進6桁以内、所要時間は正
            return null;
        int speed = (int)(Math.Abs(angle) / 100 / time);
        if (0xFFFF < speed) // 移動速度は16進4桁以内
            return null;
        return RotateAngle(ToString(motor), Orientation(angle), Math.Abs(angle), speed);
    }
}
}
