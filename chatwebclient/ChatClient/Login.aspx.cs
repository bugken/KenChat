﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Net;
using System.IO;
using System.Web.Script.Serialization;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace ChatClient
{
    class LoginInfo
    {
        public int msgid { get; set; }
        public int id { get; set; }
        public string password { get; set; }
    }
    class LoginReplyInfo
    {
        public int errno { get; set; }
        public string errmsg { get; set; }
        public int msgid { get; set; }
        public int id { get; set; }
        public string name { get; set; }
        /*Note:服务端返回friend信息是字符串的形式*/
        public List<string> friends { get; set; }
        /*Note:服务端返回group信息是字符串的形式*/
        public List<string> groups { get; set; }
    }
    class FriendsInfo
    {
        public int id { get; set; }
        public string name { get; set; }
        public string state { get; set; }
    }
    public class GroupsInfo
    {
        public int id { get; set; }
        public string groupname { get; set; }
        public string groupdesc { get; set; }
    }
    public partial class Login : System.Web.UI.Page
    {
        private string URLs = "http://192.168.107.128:8000";
        private JavaScriptSerializer JsonParserJss;
        private LoginInfo loginInfo;
        private HttpWebRequest httpWebRequest;
        public Login()
        {
            System.Diagnostics.Debug.WriteLine("Login");
            loginInfo = new LoginInfo();
            loginInfo.msgid = 1;
            loginInfo.id = 0;
            loginInfo.password = "123456";
            JsonParserJss = new JavaScriptSerializer();

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
                string json = JsonParserJss.Serialize(loginInfo);
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
            string json = JsonParserJss.Serialize(loginInfo);
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
            byte[] jsonData = Encoding.UTF8.GetBytes(JsonParserJss.Serialize(loginInfo));
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
        
            loginInfo.id = int.Parse(UserName.Text);
            loginInfo.password = Password.Text;

            string DataBuff = "";
            if (LoginByWebClient(ref DataBuff))
            //if (LoginByHttpWebRequestAndWrite(ref DataBuff))
            //if (LoginByHttpWebRequestAndStream(ref DataBuff))
            {
                /*解析Json数据，并进入聊天Lobby*/
                System.Diagnostics.Debug.WriteLine(DataBuff);
                //LoginReplyInfo LoginReplyData = JsonParserJss.Deserialize<LoginReplyInfo>(DataBuff);
                LoginReplyInfo LoginReplyData = JsonConvert.DeserializeObject<LoginReplyInfo>(DataBuff);
                if (LoginReplyData.errno == 0)
                {
                    if (LoginReplyData.friends != null)
                    {
                        List<string> Friends = LoginReplyData.friends;
                        foreach (string Item in Friends)
                        {
                            //FriendsInfo fellow = JsonParserJss.Deserialize<FriendsInfo>(Item);
                            FriendsInfo fellow = JsonConvert.DeserializeObject<FriendsInfo>(Item);
                        }
                    }
                    if (LoginReplyData.groups != null)
                    {
                        List<string> Groups = LoginReplyData.groups;
                        foreach (string Item in Groups)
                        {
                            //GroupsInfo group = JsonParserJss.Deserialize<GroupsInfo>(Item);
                            GroupsInfo group = JsonConvert.DeserializeObject<GroupsInfo>(Item);
                        }
                    }
                    Response.Redirect("ChatLobby.aspx");
                }
                else 
                {
                    Response.Write("<script>alert('" + LoginReplyData.errmsg + "')</script>");
                }
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