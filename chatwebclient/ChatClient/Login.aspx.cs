using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Net;
using System.IO;
using System.Web.Script.Serialization;
using System.Text;

namespace ChatClient
{
    public partial class Login : System.Web.UI.Page
    {
        private string URLs = "http://192.168.107.128:8000";
        private string JsonStr = "{\"msgid\":1,\"id\":1, \"password\":\"123456\"}";
        private string SendHttpRequest()
        {
            System.Diagnostics.Debug.WriteLine("SendHttpRequest");
            HttpWebRequest httpWebRequest = (HttpWebRequest)WebRequest.Create(URLs);
            httpWebRequest.ContentType = "application/json";
            httpWebRequest.Method = "POST";
            httpWebRequest.UserAgent = null;
            httpWebRequest.Timeout = 1000;
            ServicePointManager.SecurityProtocol = SecurityProtocolType.Tls12 | SecurityProtocolType.Tls11 | SecurityProtocolType.Tls;
 
            using (var streamWriter = new StreamWriter(httpWebRequest.GetRequestStream()))
            {
                string json = new JavaScriptSerializer().Serialize(new
                {
                    msgid = 1,
                    id = 1,
                    password = "123456"
                });
                streamWriter.Write(json);
                streamWriter.Flush();
                streamWriter.Close();
            }

            string retString;
            HttpWebResponse httpResponse = (HttpWebResponse)httpWebRequest.GetResponse();
            using (var streamReader = new StreamReader(httpResponse.GetResponseStream()))
            {
                retString = streamReader.ReadToEnd();
            }
            return retString;
        }
        private string SendHttpRequest2()
        {
            System.Diagnostics.Debug.WriteLine("SendHttpRequest2");
            var httpWebRequest = (HttpWebRequest)WebRequest.Create(URLs);
            httpWebRequest.ContentType = "application/json";
            httpWebRequest.Method = "POST";
            httpWebRequest.UserAgent = null;
            httpWebRequest.Timeout = 1000;
            ServicePointManager.SecurityProtocol = SecurityProtocolType.Tls12 | SecurityProtocolType.Tls11 | SecurityProtocolType.Tls;
            string json = new JavaScriptSerializer().Serialize(new
            {
                msgid = 1,
                id = 1,
                password = "123456"
            });
            byte[] postBytes = Encoding.ASCII.GetBytes(json);
            httpWebRequest.ContentLength = postBytes.Length;
            using (Stream reqStream = httpWebRequest.GetRequestStream())
            {
                reqStream.Write(postBytes, 0, postBytes.Length);
            }

            string retString;
            HttpWebResponse httpResponse = (HttpWebResponse)httpWebRequest.GetResponse();
            using (var streamReader = new StreamReader(httpResponse.GetResponseStream()))
            {
                retString = streamReader.ReadToEnd();
            }
            return retString;
        }
        private string SendHttpRequest3()
        {
            System.Diagnostics.Debug.WriteLine("SendHttpRequest3");
            byte[] jsonData = Encoding.UTF8.GetBytes(JsonStr);

            WebClient client = new WebClient();
            client.Headers.Add("Content-Type", "application/json"); //采取POST方式必须加的header
            client.Headers.Add("ContentLength", jsonData.Length.ToString());
            byte[] responseData = client.UploadData(URLs, "POST", jsonData); //得到返回字符流
            string retString = Encoding.UTF8.GetString(responseData); //解码

            return retString;
        }

        protected void Page_Load(object sender, EventArgs e)
        {

        }
        protected void BtnLoginClick(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("BtnLoginClick");
            Response.Write("BtnLoginClick");
            SendHttpRequest3();
        }
        protected void LinkBtnToRegisterClick(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("BtnLoginClick");
            Response.Write("LinkBtnToRegisterClick");
            Response.Redirect("Register.aspx");
        }
    }
}