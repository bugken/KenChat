using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Net;
using System.Web.Script.Serialization;
using System.IO;
using System.Text;

namespace ChatClient
{
    public class Client
    {
        public int UserID { get; set; }
        public string UserName { get; set; }
    }
    public class QueryMsg
    {
        public int msgid { get; set;}
        public int id { get; set;}
    }
    public class OfflineMsgBuff
    {
        public int msgid { get; set; }
        public List<string> offlinemessage { get; set; }
    }
    public class OfflineMsg
    {
        public int toid { get; set; }
        public int id { get; set; }
        public string name { get; set; }
        public string message { get; set; }
        public string time { get; set; }
        public int msgid { get; set; }
    }
    public class FriendMsgBuff
    {
        public int msgid { get; set; }
        public List<string> friends { get; set; }
    }
    public class FriendMsg
    {
        public int id { get; set; }
        public string name { get; set; }
        public string state { get; set; }
    }
    public class GroupMsgBuff
    {
        public int msgid { get; set; }
        public List<string> groups { get; set; }
    }
    public class GroupMsg
    {
        public int id { get; set; }
        public string groupname { get; set; }
        public string groupdesc { get; set; }
    }
    public partial class ChatLobby : System.Web.UI.Page
    {
        private string URLs = "http://192.168.107.128:8000";
        private JavaScriptSerializer JsonParserJss;
        private QueryMsg queryMsg;
        private Client client;
        public ChatLobby()
        {
            System.Diagnostics.Debug.WriteLine("ChatLobby");

            client = new Client();
            queryMsg = new QueryMsg();
            JsonParserJss = new JavaScriptSerializer();
        }
        private bool QueryInfo(ref string DataBuff, int MsgID)
        {
            System.Diagnostics.Debug.WriteLine("QueryInfo");
            queryMsg.id = client.UserID;
            queryMsg.msgid = MsgID;

            bool Result = true;
            HttpWebRequest httpWebRequest = (HttpWebRequest)WebRequest.Create(URLs);
            httpWebRequest.Method = "POST";
            httpWebRequest.ContentType = "application/json";
            httpWebRequest.UserAgent = null;
            httpWebRequest.Timeout = 1000;
            using (var streamWriter = new StreamWriter(httpWebRequest.GetRequestStream()))
            {
                string json = JsonParserJss.Serialize(queryMsg);
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
        private bool ShowOfflineMsg()
        {
            System.Diagnostics.Debug.WriteLine("ShowOfflineMsg");
            bool Result = true ;
            int msgId = 11;
            string offlinebuff = "";
            if (QueryInfo(ref offlinebuff, msgId))
            {
                OfflineMsgBuff strMessageBuff = JsonParserJss.Deserialize<OfflineMsgBuff>(offlinebuff);
                if (strMessageBuff.offlinemessage != null)
                {
                    foreach (string Item in strMessageBuff.offlinemessage)
                    {
                        OfflineMsg message = JsonParserJss.Deserialize<OfflineMsg>(Item);
                        //显示到ChatLobby
                        string offlineMsg = "[" + message.time + "]" + "[" + message.name 
                                             + "(" + message.id.ToString() + ")" 
                                             + "]:" + message.message;
                        TxtAllMsg.Text = TxtAllMsg.Text + offlineMsg + "\r\n";
                    }
                }
            }
            else 
            {
                Result = false;
                Response.Write("<script>alert('获取离线信息失败!')</script>");
            }

            return Result;
        }
        private bool ShowFriendsMsg()
        {
            System.Diagnostics.Debug.WriteLine("ShowFriendsMsg");
            bool Result = true;
            int msgId = 13;
            string friendsbuff = "";
            if (QueryInfo(ref friendsbuff, msgId))
            {
                FriendMsgBuff strMessageBuff = JsonParserJss.Deserialize<FriendMsgBuff>(friendsbuff);
                if (strMessageBuff.friends != null)
                {
                    foreach (string Item in strMessageBuff.friends)
                    {
                        FriendMsg message = JsonParserJss.Deserialize<FriendMsg>(Item);
                        //显示到ChatLobby
                        ListFriends.Items.Clear();
                        ListFriends.Items.Add(message.id.ToString() + ":" + message.name + ":" + message.state);
                    }
                }
            }
            else
            {
                Result = false;
                Response.Write("<script>alert('获取好友信息失败!')</script>");
            }

            return Result;
        }
        private bool ShowGroupsMsg()
        {
            System.Diagnostics.Debug.WriteLine("ShowGroupsMsg");
            bool Result = true;
            int msgId = 15;
            string groupsbuff = "";
            if (QueryInfo(ref groupsbuff, msgId))
            {
                GroupMsgBuff strMessageBuff = JsonParserJss.Deserialize<GroupMsgBuff>(groupsbuff);
                if (strMessageBuff.groups != null)
                {
                    foreach (string Item in strMessageBuff.groups)
                    {
                        GroupMsg message = JsonParserJss.Deserialize<GroupMsg>(Item);
                        //显示到ChatLobby
                        ListGroups.Items.Clear();
                        ListGroups.Items.Add(message.id.ToString() + ":" + message.groupname);
                    }
                }
            }
            else
            {
                Result = false;
                Response.Write("<script>alert('获取群组信息失败!')</script>");
            }

            return Result;
        }
        protected void Page_Load(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("Page_Load");
            string PageSource = Request["PageFrom"];
            string UserID = Request["UserID"];
            string UserName = Request["UserName"];
            if (UserID != null && PageSource != null && UserName != null)
            {
                System.Diagnostics.Debug.WriteLine("From Page " + PageSource);

                client.UserID = int.Parse(UserID);
                client.UserName = UserName;
                TxtUserID.Text = "用户ID:" + UserID + " 用户名:" + UserName;
                
                if (PageSource == "Login")
                {
                    //显示离线消息
                    //ShowOfflineMsg();
                    //显示Friends信息
                    ShowFriendsMsg();
                    //显示Groups信息
                    ShowGroupsMsg();
                }
                else if(PageSource == "Register")
                {
                    
                }
            }
        }
        protected void BtnSendClick(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("BtnSendClick");
            ShowOfflineMsg();
        }
    }
}