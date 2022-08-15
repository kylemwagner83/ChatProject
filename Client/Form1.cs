using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Client
{
    public partial class Form1 : Form
    {
        TcpClient _client;
        byte[] _buffer = new byte[4096];


        public Form1()
        {
            InitializeComponent();
        }


        // ***** CLICK CONNECT BUTTON
        private void connectButton_Click(object sender, EventArgs e)
        {
            // TODO: Add error handling for invalid IP/port/failure to connect/already connected
            _client = new TcpClient();
            _client.Connect(ipTextBox.Text, int.Parse(portTextBox.Text));
            _client.GetStream().BeginRead(_buffer, 0, _buffer.Length, Server_MessageReceived, null);
        }


        // ***** TODO: Add disconnect button? *****


        // ***** RECEIVE MESSAGE FROM SERVER *****
        private void Server_MessageReceived(IAsyncResult ar)
        {
            if (ar.IsCompleted)
            {
                var bytesIn = _client.GetStream().EndRead(ar);
                if (bytesIn > 0)
                {
                    var tmp = new byte[bytesIn];
                    Array.Copy(_buffer, 0, tmp, 0, bytesIn);

                    var str = Encoding.ASCII.GetString(tmp);
                    BeginInvoke((Action)(() =>
                    {
                        displayBox.Items.Add(str);
                        displayBox.SelectedIndex = displayBox.Items.Count - 1;
                    }));
                }
                Array.Clear(_buffer, 0, _buffer.Length);
                _client.GetStream().BeginRead(_buffer, 0, _buffer.Length, Server_MessageReceived, null);
            }
        }

 
        // ***** CLICK SEND BUTTON *****
        private void sendButton_Click(object sender, EventArgs e)
        {
            // TODO: Add error handling if clicked when not connected to server
            var msg = Encoding.ASCII.GetBytes(inputTextBox.Text);
            _client.GetStream().Write(msg, 0, msg.Length);
            inputTextBox.Text = "";
        }

        

    }
}
