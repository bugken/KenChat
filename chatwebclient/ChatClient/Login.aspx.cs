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
    public class LoginInfo
    {
        public int msgid { get; set; }
        public int id { get; set; }
        public string password { get; set; }
    }
    public partial class Login : System.Web.UI.Page
    {
        private string URLs = "http://192.168.107.128:8000";
        private string JsonStr = "{\"msgid\":1,\"id\":1, \"password\":\"123456\"}";
        private JavaScriptSerializer Jss;
        private LoginInfo loginInfo;
        private HttpWebRequest httpWebRequest;
        public Login()
        {
            System.Diagnostics.Debug.WriteLine("Login");
            loginInfo = new LoginInfo();
            loginInfo.msgid = 1;
            loginInfo.id = 1;
            loginInfo.password = "123456";
            Jss = new JavaScriptSerializer();

            httpWebRequest = (HttpWebRequest)WebRequest.Create(URLs);
            httpWebRequest.Method = "POST";
            httpWebRequest.ContentType = "application/json";
            httpWebRequest.UserAgent = null;
            httpWebRequest.Timeout = 1000;
        }
        ~Login()
        {
            System.Diagnostics.Debug.WriteLine("~Login");
        }
        private bool LoginByHttpWebRequestAndStream(ref string DataBuff)
        {
            System.Diagnostics.Debug.WriteLine("LoginByHttpWebRequestAndStream");

            bool Result = true;
            using (var streamWriter = new StreamWriter(httpWebRequest.GetRequestStream()))
            {
                string json = Jss.Serialize(loginInfo);
                streamWriter.Write(json);
                streamWriter.Flush();
                streamWriter.Close();
            }

            try
            {
                HttpWebResponse httpResponse = (HttpWebResponse)httpWebRequest.GetResponse();
                using (var streamReader = new StreamReader(httpResponse.GetResponseStream()))
                {
                    DataBuff = streamReader.ReadToEnd();
                }
            }
            catch (Exception ex)
            {
                Response.Write("<script>alert('(" + ex.GetType().Name + ")" + ex.Message + "')</script>");
                DataBuff = ex.Message;
                Result = false;
            }
            return Result;
        }
        private bool LoginByHttpWebRequestAndWrite(ref string DataBuff)
        {
            System.Diagnostics.Debug.WriteLine("LoginByHttpWebRequestAndWrite");

            bool Result = true;
            string json = Jss.Serialize(loginInfo);
            byte[] postBytes = Encoding.ASCII.GetBytes(json);
            httpWebRequest.ContentLength = postBytes.Length;

            try
            {
                using (Stream reqStream = httpWebRequest.GetRequestStream())
                {
                    reqStream.Write(postBytes, 0, postBytes.Length);
                }
                HttpWebResponse httpResponse = (HttpWebResponse)httpWebRequest.GetResponse();
                using (var streamReader = new StreamReader(httpResponse.GetResponseStream()))
                {
                    DataBuff = streamReader.ReadToEnd();
                }
            }
            catch (Exception ex)
            {
                Response.Write("<script>alert('(" + ex.GetType().Name + ")" + ex.Message + "')</script>");
                DataBuff = ex.Message;
                Result = false;
            }

            return Result;
        }
        private bool LoginByWebClient(ref string DataBuff)
        {
            System.Diagnostics.Debug.WriteLine("LoginByWebClient");

            bool Result = true;
            WebClient client = new WebClient();
            byte[] jsonData = Encoding.UTF8.GetBytes(JsonStr);
            client.Headers.Add("Content-Type", "application/json"); //采取POST方式必须加的header
            client.Headers.Add("ContentLength", jsonData.Length.ToString());
            try
            {
                byte[] responseData = client.UploadData(URLs, "POST", jsonData); //得到返回字符流
                DataBuff = Encoding.UTF8.GetString(responseData); //解码
            }
            catch (Exception ex)
            {
                Response.Write("<script>alert('(" + ex.GetType().Name + ")" + ex.Message + "')</script>");
                DataBuff = ex.Message;
                Result = false;
            }

            return Result;
        }

        protected void Page_Load(object sender, EventArgs e)
        {

        }
        protected void BtnLoginClick(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("BtnLoginClick");

            string DataBuff = "";
            //if (LoginByWebClient(ref DataBuff))
            if (LoginByHttpWebRequestAndWrite(ref DataBuff))
            //if (LoginByHttpWebRequestAndStream(ref DataBuff))
            {
                Response.Write(DataBuff);
                Response.Write("BtnLoginClick Success");
            }
            else 
            {
                Response.Write("BtnLoginClick Failed");
            }
        }
        protected void LinkBtnToRegisterClick(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("LinkBtnToRegisterClick");
            Response.Write("LinkBtnToRegisterClick");
            Response.Redirect("Register.aspx");
        }
    }
}