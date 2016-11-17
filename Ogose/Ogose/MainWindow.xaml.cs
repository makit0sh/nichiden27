﻿using Ogose.Properties;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Ogose
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        /// <summary>
        /// メインウィンドウ及びアイテムの出現を指示
        /// </summary>
        public MainWindow()
        {
            InitializeComponent();
           
        }

        SerialPort serialPort = null;

        NisshuidohenController nisshuidohenController = new NisshuidohenController();

        const int SPEED_HIGH_DIURNAL = 1000;                   //4  deg/s
        const int SPEED_HIGH_LATITUDE = 1000 * 8192 / 9000;    //1  deg/s
        const int SPEED_LOW_DIURNAL = 250;                     //1  deg/s
        const int SPEED_LOW_LATITUDE = 500 * 8192 / 9000;      //0.5deg/s

        /// <summary>
        /// シリアルポート名Nameを取得し正規表現に合致するかを確認しシリアルポート名を表示する
        /// </summary>
        public class SerialPortItem
        {
            public string Name { get; set; }
            public string DisplayString
            {
                get
                {
                    Match m = Regex.Match(Name, @"^port(\d+)$", RegexOptions.IgnoreCase);
                    if (m.Success) return "ぽーと" + m.Groups[1];
                    else return Name;
                }
            }
            public override string ToString()
            {
                return DisplayString;
            }
        }

        /// <summary>
        /// シリアルポート名を取得し前回接続したものがあればそれを使用 ボーレートの設定
        /// </summary>
        /// <param name="ports[]">取得したシリアルポート名の配列</param>
        /// <param name="port">ports[]の要素</param>
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            var ports = SerialPort.GetPortNames();
            foreach (var port in ports)
            {
                portComboBox.Items.Add(new SerialPortItem { Name = port });
            }
            if (portComboBox.Items.Count > 0)
            {
                if (ports.Contains(Settings.Default.LastConnectedPort))
                    portComboBox.SelectedIndex = Array.IndexOf(ports, Settings.Default.LastConnectedPort);
                else
                    portComboBox.SelectedIndex = 0;
            }
            serialPort = new SerialPort
            {
                BaudRate = 2400
            };
        }

        /// <summary>
        /// PortComboBoxが空でなくConnectButtonがチェックされている時にシリアルポートの開閉を行う シリアルポートの開閉時に誤動作が発生しないよう回避している
        /// </summary>
        private void ConnectButton_IsCheckedChanged(object sender, RoutedEventArgs e)
        {
            var item = portComboBox.SelectedItem as SerialPortItem;
            if (item != null && ConnectButton.IsChecked.HasValue)
            {
                bool connecting = ConnectButton.IsChecked.Value;
                ConnectButton.Checked -= ConnectButton_IsCheckedChanged;
                ConnectButton.Unchecked -= ConnectButton_IsCheckedChanged;
                ConnectButton.IsChecked = null;

                if (serialPort != null && serialPort.IsOpen) serialPort.Close();
                if (serialPort != null && connecting)
                {

                    serialPort.PortName = item.Name;
                    try
                    {
                        serialPort.Open();
                    }
                    catch (IOException ex)
                    {
                        ConnectButton.IsChecked = false;
                        MessageBox.Show(ex.ToString(), ex.GetType().Name);
                        return;
                    }
                    catch (UnauthorizedAccessException ex)
                    {
                        ConnectButton.IsChecked = false;
                        MessageBox.Show(ex.ToString(), ex.GetType().Name);
                        return;
                    }
                    Settings.Default.LastConnectedPort = item.Name;
                    Settings.Default.Save();
                }

                ConnectButton.IsChecked = connecting;
                ConnectButton.Checked += ConnectButton_IsCheckedChanged;
                ConnectButton.Unchecked += ConnectButton_IsCheckedChanged;
                portComboBox.IsEnabled = !connecting;
            }
            else
            {
                ConnectButton.IsChecked = false;
            }
        }


        /// <summary>
        /// シリアルポート名を取得（2つ前のメソッドにほぼ同様の記述あり）
        /// </summary>
        private void portComboBox_DropDownOpened(object sender, EventArgs e)
        {
            var item = portComboBox.SelectedItem as SerialPortItem;
            portComboBox.SelectedIndex = -1;
            portComboBox.Items.Clear();
            string[] ports = SerialPort.GetPortNames();
            foreach (var port in ports)
                portComboBox.Items.Add(new SerialPortItem { Name = port });
            if (item != null && ports.Contains(item.Name))
                portComboBox.SelectedIndex = Array.IndexOf(ports, item.Name);
        }

        /// <summary>
        /// シリアルポートが開いている時にコマンドcmdをシリアルポートに書き込み閉じている時はMassageBoxを表示する
        /// </summary>
        /// <param name="cmd"></param>
        private void emitCommand(string cmd)
        {
            if (serialPort.IsOpen)
            {
                var bytes = Encoding.ASCII.GetBytes(cmd);
                serialPort.RtsEnable = true;
                serialPort.Write(bytes, 0, bytes.Length);
                Thread.Sleep(100);
                serialPort.RtsEnable = false;
            }

            else
            {
                MessageBox.Show(cmd, "cmd");
            }

        }

        private void diurnalPlusButton_Click(object sender, RoutedEventArgs e)
        {
            emitCommand(nisshuidohenController.RotateDiurnalBySpeed(-SPEED_HIGH_DIURNAL));
            diurnalPlusButton.Focus();
        }
        
        private void diurnalMinusButton_Click(object sender, RoutedEventArgs e)
        {
            emitCommand(nisshuidohenController.RotateDiurnalBySpeed(SPEED_HIGH_DIURNAL));
            diurnalMinusButton.Focus();
        }
        
        private void diurnalStopButton_Click(object sender, RoutedEventArgs e)
        {
            emitCommand(nisshuidohenController.RotateDiurnalBySpeed(0));
            diurnalStopButton.Focus();
        }

        private void latitudePlusButtonn_Click(object sender, RoutedEventArgs e)
        {
            emitCommand(nisshuidohenController.RotateLatitudeBySpeed(SPEED_HIGH_LATITUDE));
            latitudePlusButton.Focus();
        }

        private void latitudeMinusButton_Click(object sender, RoutedEventArgs e)
        {
            emitCommand(nisshuidohenController.RotateLatitudeBySpeed(-SPEED_HIGH_LATITUDE));
            latitudeMinusButton.Focus();
        }

        private void latitudeStopButton_Click(object sender, RoutedEventArgs e)
        {
            emitCommand(nisshuidohenController.RotateLatitudeBySpeed(0));
            latitudeStopButton.Focus();
        }

        private void checkBox1_Checked(object sender, RoutedEventArgs e)
        {
            this.window1.WindowStyle = WindowStyle.None;
            this.window1.WindowState = WindowState.Maximized;
            this.window1.Topmost = true;
        }

        private void checkBox1_Unchecked(object sender, RoutedEventArgs e)
        {
            this.window1.WindowStyle = WindowStyle.SingleBorderWindow;
            this.window1.WindowState = WindowState.Normal;
            this.window1.Topmost = false;
        }
    }
}