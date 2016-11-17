using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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
        public MainWindow()
        {
            InitializeComponent();
           
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