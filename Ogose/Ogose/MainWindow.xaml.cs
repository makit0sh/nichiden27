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
using System.Windows.Input;

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
            initCommandBindings();

        }
        /// <summary> シリアルポートを使用するので宣言 </summary>
        SerialPort serialPort = null;
        /// <summary> コントローラと通信するためのクラスNisshuidohenControllerのインスタンス </summary>
        NisshuidohenController nisshuidohenController = new NisshuidohenController();

        /// <summary> RoutedCommand </summary>
        public readonly static RoutedCommand diurnalPlusButtonCommand = new RoutedCommand("diurnalPlusButtonCommand", typeof(MainWindow));
        public readonly static RoutedCommand diurnalMinusButtonCommand = new RoutedCommand("diurnalMinusButtonCommand", typeof(MainWindow));
        public readonly static RoutedCommand latitudePlusButtonCommand = new RoutedCommand("latitudePlusButtonCommand", typeof(MainWindow));
        public readonly static RoutedCommand latitudeMinusButtonCommand = new RoutedCommand("latitudeMinusButtonCommand", typeof(MainWindow));

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

        /// <summary> 各ボタンが操作できるかどうかを記憶 </summary>
        private Dictionary<string, bool> isEnabled = new Dictionary<string, bool>()
        {
            {"diurnalPlusButton", true},
            {"diurnalMinusButton", true},
            {"latitudePlusButton", true},
            {"latitudeMinusButton", true}
        };
        /// <summary> 公演モードの管理 </summary>
        private bool isPerfMode = false;


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
                        serialPort.WriteTimeout = 500;
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

        private void selectRadioButton(string gridName, int direction)
        {
            var buttons = ((Grid)FindName(gridName)).Children;
            foreach(RadioButton item in buttons)
            {
                MessageBox.Show(item.Content.ToString());
            }
        }

        private void diurnalRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            var radioButton = (RadioButton)sender;
            if (radioButton.Name == "diurnalRadioButton1") diurnal_speed = SPEED_DIURNAL["very_high"];
            else if (radioButton.Name == "diurnalRadioButton2") diurnal_speed = SPEED_DIURNAL["high"];
            else if (radioButton.Name == "diurnalRadioButton3") diurnal_speed = SPEED_DIURNAL["low"];
            else if (radioButton.Name == "diurnalRadioButton4") diurnal_speed = SPEED_DIURNAL["very_low"];

            if (diurnalPlusButton.IsChecked == true)
                diurnalPlusButtonCommand.Execute(null, diurnalPlusButton);
            if (diurnalMinusButton.IsChecked == true)
                diurnalMinusButtonCommand.Execute(null, diurnalMinusButton);
        }

        private void latitudeRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            var radioButton = (RadioButton)sender;
            if (radioButton.Name == "latitudeRadioButton1") latitude_speed = SPEED_LATITUDE["very_high"];
            else if (radioButton.Name == "latitudeRadioButton2") latitude_speed = SPEED_LATITUDE["high"];
            else if (radioButton.Name == "latitudeRadioButton3") latitude_speed = SPEED_LATITUDE["low"];
            else if (radioButton.Name == "latitudeRadioButton4") latitude_speed = SPEED_LATITUDE["very_low"];

            if (latitudePlusButton.IsChecked == true)
                latitudePlusButtonCommand.Execute(null, latitudePlusButton);
            if (latitudeMinusButton.IsChecked == true)
                latitudeMinusButtonCommand.Execute(null, latitudeMinusButton);
        }

        /// <summary>
        /// 逆向きの回転を行うボタンの有効/無効を切り替える
        /// </summary>
        /// <param name="button"></param>
        private void toggleOppositeButton(ToggleButton button)
        {
            String oppositeButton = "";
            if (button.Name == "diurnalPlusButton") oppositeButton = "diurnalMinusButton";
            else if (button.Name == "diurnalMinusButton") oppositeButton = "diurnalPlusButton";
            else if (button.Name == "latitudePlusButton") oppositeButton = "latitudeMinusButton";
            else if (button.Name == "latitudeMinusButton") oppositeButton = "latitudePlusButton";
            isEnabled[oppositeButton] = !isEnabled[oppositeButton] && !isPerfMode;
            (button).Focus();
        }

        /// <summary>
        /// MainWindowに必要なコマンドを追加する。コンストラクタで呼び出して下さい
        /// </summary>
        private void initCommandBindings()
        {
            diurnalPlusButton.CommandBindings.Add(new CommandBinding(diurnalPlusButtonCommand, diurnalPlusButtonCommand_Executed, toggleButton_CanExecuted));
            diurnalMinusButton.CommandBindings.Add(new CommandBinding(diurnalMinusButtonCommand, diurnalMinusButtonCommand_Executed, toggleButton_CanExecuted));
            latitudePlusButton.CommandBindings.Add(new CommandBinding(latitudePlusButtonCommand, latitudePlusButtonCommand_Executed, toggleButton_CanExecuted));
            latitudeMinusButton.CommandBindings.Add(new CommandBinding(latitudeMinusButtonCommand, latitudeMinusButtonCommand_Executed, toggleButton_CanExecuted));
        }

        private void toggleButton_CanExecuted(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = isEnabled[((ToggleButton)sender).Name];
        }

        private void diurnalPlusButtonCommand_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (e.Parameter != null && e.Parameter.ToString() == "KeyDown")
            {
                ((ToggleButton)sender).IsChecked = !((ToggleButton)sender).IsChecked;
            }
            if (sender as ToggleButton != null && ((ToggleButton)sender).IsChecked == false)
            {
                emitCommand(nisshuidohenController.RotateDiurnalBySpeed(0));
            }
            else
            {
                emitCommand(nisshuidohenController.RotateDiurnalBySpeed(diurnal_speed));
            }
            if (sender as ToggleButton != null) toggleOppositeButton((ToggleButton)sender);
        }

        private void diurnalMinusButtonCommand_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (e.Parameter != null && e.Parameter.ToString() == "KeyDown")
            {
                ((ToggleButton)sender).IsChecked = !((ToggleButton)sender).IsChecked;
            }
            if (sender as ToggleButton != null && ((ToggleButton)sender).IsChecked == false)
            {
                emitCommand(nisshuidohenController.RotateDiurnalBySpeed(0));
            }
            else
            {
                emitCommand(nisshuidohenController.RotateDiurnalBySpeed(-diurnal_speed));
            }
            if (sender as ToggleButton != null) toggleOppositeButton((ToggleButton)sender);
        }

        private void latitudePlusButtonCommand_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (e.Parameter != null && e.Parameter.ToString() == "KeyDown")
            {
                ((ToggleButton)sender).IsChecked = !((ToggleButton)sender).IsChecked;
            }
            if (sender as ToggleButton != null && ((ToggleButton)sender).IsChecked == false)
            {
                emitCommand(nisshuidohenController.RotateLatitudeBySpeed(0));
            }
            else
            {
                emitCommand(nisshuidohenController.RotateLatitudeBySpeed(latitude_speed));
            }
            if (sender as ToggleButton != null) toggleOppositeButton((ToggleButton)sender);
        }

        private void latitudeMinusButtonCommand_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (e.Parameter != null && e.Parameter.ToString() == "KeyDown")
            {
                ((ToggleButton)sender).IsChecked = !((ToggleButton)sender).IsChecked;
            }
            if (sender as ToggleButton != null && ((ToggleButton)sender).IsChecked == false)
            {
                emitCommand(nisshuidohenController.RotateLatitudeBySpeed(0));
            }
            else
            {
                emitCommand(nisshuidohenController.RotateLatitudeBySpeed(-latitude_speed));
            }
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

        private void checkBox2_Changed(object sender, RoutedEventArgs e)
        {
            var result = new MessageBoxResult();
            isPerfMode = (bool)(((CheckBox)sender).IsChecked);
            if(isPerfMode)
            {
              result = MessageBox.Show("公演モードに切り替えます。\n日周を進める以外の動作はロックされます。よろしいですか？", "Changing Mode", MessageBoxButton.YesNo);
            }
            else
            {
              result = MessageBox.Show("公演モードを解除します。\nよろしいですか？", "Changing Mode", MessageBoxButton.YesNo);
            }
            if(result == MessageBoxResult.No) return;
            List<string> keyList = new List<string>(isEnabled.Keys); // isEnabled.Keysを直接見に行くとループで書き換えてるので実行時エラーになる
            foreach (string key in keyList)
            {
                if(key != "diurnalMinusButton") isEnabled[key] = !isPerfMode;
            }
            latitudeRadioButton1.IsEnabled = latitudeRadioButton2.IsEnabled = latitudeRadioButton3.IsEnabled = latitudeRadioButton4.IsEnabled = !isPerfMode;
        }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            var target = new ToggleButton();
            switch (e.Key)
            {
                case Key.W:
                    latitudePlusButtonCommand.Execute("KeyDown", latitudePlusButton);
                    break;
                case Key.A:
                    diurnalPlusButtonCommand.Execute("KeyDown", diurnalPlusButton);
                    break;
                case Key.S:
                    latitudeMinusButtonCommand.Execute("KeyDown", latitudeMinusButton);
                    break;
                case Key.D:
                    diurnalMinusButtonCommand.Execute("KeyDown", diurnalMinusButton);
                    break;
            }
        }
    }
}
