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
    public class ChatMsg
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
        private string m_URLs = "http://192.168.107.128:8000";
        private JavaScriptSerializer m_JsonParserJss = new JavaScriptSerializer();
        private static int m_UserID = 0;
        private static string m_UserName = "";
        private static int m_ToUserID = 0;
        public ChatLobby()
        {
            System.Diagnostics.Debug.WriteLine("ChatLobby");
        }
        private bool QueryInfo(ref string DataBuff, int MsgID)
        {
            System.Diagnostics.Debug.WriteLine("QueryInfo");
            QueryMsg queryMsg = new QueryMsg();
            queryMsg.id = m_UserID;
            queryMsg.msgid = MsgID;

            bool Result = true;
            HttpWebRequest httpWebRequest = (HttpWebRequest)WebRequest.Create(m_URLs);
            httpWebRequest.Method = "POST";
            httpWebRequest.ContentType = "application/json";
            httpWebRequest.UserAgent = null;
            httpWebRequest.Timeout = 1000;
            using (var streamWriter = new StreamWriter(httpWebRequest.GetRequestStream()))
            {
                string json = m_JsonParserJss.Serialize(queryMsg);
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
                OfflineMsgBuff strMessageBuff = m_JsonParserJss.Deserialize<OfflineMsgBuff>(offlinebuff);
                if (strMessageBuff.offlinemessage != null)
                {
                    foreach (string Item in strMessageBuff.offlinemessage)
                    {
                        ChatMsg message = m_JsonParserJss.Deserialize<ChatMsg>(Item);
                        //显示到ChatLobby
                        string offlineMsg = "[" + message.time + "]" + "[" + message.name 
                                             + "(" + message.id.ToString() + ")" 
                                             + "][离线消息]:" + message.message;
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
                FriendMsgBuff strMessageBuff = m_JsonParserJss.Deserialize<FriendMsgBuff>(friendsbuff);
                if (strMessageBuff.friends != null)
                {
                    ListFriends.Items.Clear();
                    foreach (string Item in strMessageBuff.friends)
                    {
                        FriendMsg message = m_JsonParserJss.Deserialize<FriendMsg>(Item);
                        //显示到ChatLobby
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
                GroupMsgBuff strMessageBuff = m_JsonParserJss.Deserialize<GroupMsgBuff>(groupsbuff);
                if (strMessageBuff.groups != null)
                {
                    ListGroups.Items.Clear();
                    foreach (string Item in strMessageBuff.groups)
                    {
                        GroupMsg message = m_JsonParserJss.Deserialize<GroupMsg>(Item);
                        //显示到ChatLobby
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
        private bool OneChat(string Message)
        {
            System.Diagnostics.Debug.WriteLine("OneChat");

            bool Result = true;

            ChatMsg SendMsg = new ChatMsg();
            SendMsg.msgid = 6;
            SendMsg.id = m_UserID;
            SendMsg.name = m_UserName;
            SendMsg.message = Message;
            SendMsg.toid = m_ToUserID;
            SendMsg.time = DateTime.Now.ToString("yyyy-MM-dd") + " " + DateTime.Now.ToLongTimeString().ToString();

            string ReplyBuff = "";
            WebClient client = new WebClient();
            byte[] jsonData = Encoding.UTF8.GetBytes(m_JsonParserJss.Serialize(SendMsg));
            client.Headers.Add("Content-Type", "application/json"); //采取POST方式必须加的header
            client.Headers.Add("ContentLength", jsonData.Length.ToString());
            try
            {
                byte[] responseData = client.UploadData(m_URLs, "POST", jsonData); //得到返回字符流
                ReplyBuff = Encoding.UTF8.GetString(responseData); //解码
            }
            catch (Exception ex)
            {
                Response.Write("<script>alert('(" + ex.GetType().Name + ")" + ex.Message + "')</script>");
                ReplyBuff = ex.Message;
                Result = false;
            }

            return Result;
        }
        protected void Page_Load(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("Page_Load");
            string PageSource = Request["PageFrom"];
            string UserID = Request["UserID"];
            string UserName = Request["UserName"];
            if (IsPostBack == false && UserID != null && PageSource != null && UserName != null)
            {
                System.Diagnostics.Debug.WriteLine("From Page " + PageSource);

                m_UserID = int.Parse(UserID);
                m_UserName = UserName;

                TxtUserID.Text = "用户ID:" + UserID + " 用户名:" + UserName;
                
                if (PageSource == "Login")
                {
                    //显示离线消息
                    ShowOfflineMsg();
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
            string SendMsg = TxtMsgSend.Text;
            TxtMsgSend.Text = "";

            TxtAllMsg.Text = TxtAllMsg.Text + SendMsg + "\r\n";

            //发送消息给User
            OneChat(SendMsg);
        }
        protected void ListFriends_SelectedIndexChanged(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("ListFriends_SelectedIndexChanged");
            if(IsPostBack == true)
            {
                string SelectedUser = ListFriends.SelectedItem.Text;
                string[] ArrayUserInfo = SelectedUser.Split(':');

                string ChatStatusInfo = "你正在和用户" + ArrayUserInfo[1] + "(" + ArrayUserInfo[0] + ")" + "聊天";
                TxtChatStatus.Text = ChatStatusInfo;
                m_ToUserID = int.Parse(ArrayUserInfo[0]);
            }
        }
    }
}