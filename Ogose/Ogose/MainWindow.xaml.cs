using Ogose.Properties;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;

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
        /// <summary> シリアルポートを使用するので宣言 </summary>
        SerialPort serialPort = null;
        /// <summary> コントローラと通信するためのクラスNisshuidohenControllerのインスタンス </summary>
        NisshuidohenController nisshuidohenController = new NisshuidohenController();
        /// <summary> 日周運動で欲しいスピードのDictionary </summary>
        private static readonly Dictionary<string, double> SPEED_DIURNAL = new Dictionary<string, double>() {
            {"very_high", 6},
            {"high", 4},
            {"low", 2},
            {"very_low", 1}
        };
        /// <summary> 緯度運動で欲しいスピードのDictionary </summary>
        private static readonly Dictionary<string, double> SPEED_LATITUDE = new Dictionary<string, double>() {
            {"very_high", 2},
            {"high", 1.5},
            {"low", 1},
            {"very_low", 0.5}
        };
        /// <summary> 日周運動のスピード </summary>
        private double diurnal_speed = SPEED_DIURNAL["high"];
        /// <summary> 緯度運動のスピード </summary>
        private double latitude_speed = SPEED_LATITUDE["high"];

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

                if (serialPort.IsOpen) serialPort.Close();
                if (connecting)
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
                MessageBox.Show("Error: コントローラと接続して下さい\ncommand: "+ cmd, "Error", MessageBoxButton.OK, MessageBoxImage.Warning);
            }

        }

        private void diurnalRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            var radioButton = (RadioButton)sender;
            if (radioButton.Name == "diurnalRadioButton1") diurnal_speed = SPEED_DIURNAL["very_high"];
            else if (radioButton.Name == "diurnalRadioButton2") diurnal_speed = SPEED_DIURNAL["high"];
            else if (radioButton.Name == "diurnalRadioButton3") diurnal_speed = SPEED_DIURNAL["low"];
            else if (radioButton.Name == "diurnalRadioButton4") diurnal_speed = SPEED_DIURNAL["very_low"];

            if (diurnalPlusButton.IsChecked != null && (bool)diurnalPlusButton.IsChecked) diurnalPlusButton_Checked(new object(), new RoutedEventArgs());
            if (diurnalMinusButton.IsChecked != null && (bool)diurnalMinusButton.IsChecked) diurnalMinusButton_Checked(new object(), new RoutedEventArgs());
        }

        private void latitudeRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            var radioButton = (RadioButton)sender;
            if (radioButton.Name == "latitudeRadioButton1") latitude_speed = SPEED_LATITUDE["very_high"];
            else if (radioButton.Name == "latitudeRadioButton2") latitude_speed = SPEED_LATITUDE["high"];
            else if (radioButton.Name == "latitudeRadioButton3") latitude_speed = SPEED_LATITUDE["low"];
            else if (radioButton.Name == "latitudeRadioButton4") latitude_speed = SPEED_LATITUDE["very_low"];

            if (latitudePlusButton.IsChecked != null && (bool)latitudePlusButton.IsChecked) latitudePlusButton_Checked(new object(), new RoutedEventArgs());
            if (latitudeMinusButton.IsChecked != null && (bool)latitudeMinusButton.IsChecked) latitudeMinusButton_Checked(new object(), new RoutedEventArgs());
        }

        /// <summary>
        /// 逆向きの回転を行うボタンの有効/無効を切り替える
        /// </summary>
        /// <param name="button"></param>
        private void toggleOppositeButton(ToggleButton button)
        {
            ToggleButton oppositeButton = null;
            if (button.Name == "diurnalPlusButton") oppositeButton = (ToggleButton)FindName("diurnalMinusButton");
            else if (button.Name == "diurnalMinusButton") oppositeButton = (ToggleButton)FindName("diurnalPlusButton");
            else if (button.Name == "latitudePlusButton") oppositeButton = (ToggleButton)FindName("latitudeMinusButton");
            else if (button.Name == "latitudeMinusButton") oppositeButton = (ToggleButton)FindName("latitudePlusButton");
            oppositeButton.IsEnabled = !oppositeButton.IsEnabled;
            (button).Focus();
        }

        private void diurnalPlusButton_Checked(object sender, RoutedEventArgs e)
        {
            emitCommand(nisshuidohenController.RotateDiurnalBySpeed(diurnal_speed));
            if (sender as ToggleButton != null) toggleOppositeButton((ToggleButton)sender);
        }        
        private void diurnalMinusButton_Checked(object sender, RoutedEventArgs e)
        {
            emitCommand(nisshuidohenController.RotateDiurnalBySpeed(-diurnal_speed));
            if (sender as ToggleButton != null) toggleOppositeButton((ToggleButton)sender);
        }       
        private void diurnalButton_Unchecked(object sender, RoutedEventArgs e)
        {
            emitCommand(nisshuidohenController.RotateDiurnalBySpeed(0));
            if (sender as ToggleButton != null) toggleOppositeButton((ToggleButton)sender);
        }
        private void latitudePlusButton_Checked(object sender, RoutedEventArgs e)
        {
            emitCommand(nisshuidohenController.RotateLatitudeBySpeed(latitude_speed));
            if(sender as ToggleButton != null) toggleOppositeButton((ToggleButton)sender);
        }
        private void latitudeMinusButton_Checked(object sender, RoutedEventArgs e)
        {
            emitCommand(nisshuidohenController.RotateLatitudeBySpeed(-latitude_speed));
            if(sender as ToggleButton != null) toggleOppositeButton((ToggleButton)sender);
        }
        private void latitudeButton_Unchecked(object sender, RoutedEventArgs e)
        {
            emitCommand(nisshuidohenController.RotateLatitudeBySpeed(0));
            if (sender as ToggleButton != null) toggleOppositeButton((ToggleButton)sender);
        }

        private void checkBox1_Checked(object sender, RoutedEventArgs e)
        {
            this.window1.WindowStyle = WindowStyle.None;
            this.window1.WindowState = WindowState.Maximized;
            this.window1.Topmost = true;
            ((CheckBox)sender).Focus();
        }
        private void checkBox1_Unchecked(object sender, RoutedEventArgs e)
        {
            this.window1.WindowStyle = WindowStyle.SingleBorderWindow;
            this.window1.WindowState = WindowState.Normal;
            this.window1.Topmost = false;
        }
    }
}