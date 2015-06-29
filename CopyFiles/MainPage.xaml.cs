using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=391641

namespace CopyFiles
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();

            this.NavigationCacheMode = NavigationCacheMode.Required;
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.
        /// This parameter is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // TODO: Prepare page for display here.

            // TODO: If your application contains multiple pages, ensure that you are
            // handling the hardware Back button by registering for the
            // Windows.Phone.UI.Input.HardwareButtons.BackPressed event.
            // If you are using the NavigationHelper provided by some templates,
            // this event is handled for you.

            logBlock.Text = "Files are copied to:\r\n" +
                Windows.Storage.ApplicationData.Current.LocalFolder.Path;
        }

        private async void button_Click(object sender, RoutedEventArgs e)
        {
            logBlock.Text = "Searching for " + textBox.Text + "\r\n";
            var listing = onlyListing.IsChecked.Value;
            var obj = new CopyFilesNative.CopyFileIterator(textBox.Text, listing);
            textBox.IsEnabled = false;
            button.IsEnabled = false;
            int totalFileNumber = 0;
            UInt64 totalFileSize = 0;
            var operation = (listing) ? "Found " : "Copied ";
            try
            {
                while (true)
                {
                    var foundFile = await Task.Run(() => FindNextFile(obj));
                    if (foundFile == "")
                    {
                        break;
                    }
                    logBlock.Text += operation + foundFile + "\r\n";
                    scrollViewer.ChangeView(0, double.MaxValue, 1, true);
                    totalFileSize += obj.GetLastFileSize();
                    totalFileNumber++;
                }
                logBlock.Text += string.Format(
                    "{0}{1} file(s). Total {2} KB.",
                    operation, totalFileNumber, totalFileSize / 1024);
                scrollViewer.UpdateLayout();
                scrollViewer.ChangeView(0, double.MaxValue, 1);
            }
            finally
            {
                button.IsEnabled = true;
                textBox.IsEnabled = true;
            }
        }

        private string FindNextFile(
            CopyFilesNative.CopyFileIterator obj)
        {
            return obj.NextFile();
        }
    }
}
