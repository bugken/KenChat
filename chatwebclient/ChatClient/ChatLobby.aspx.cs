using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace ChatClient
{
    public partial class ChatLobby : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("From Page Register");
            string PageSource = Request["PageFrom"];
            string UserID = Request["UserID"];
            if (UserID != null && PageSource != null)
            {
                System.Diagnostics.Debug.WriteLine("UserID:" + UserID);
                System.Diagnostics.Debug.WriteLine("From Page Login" + PageSource);
                if (PageSource == "Login")
                {
                    //需要获取用户Friends和Groups信息以及离线消息
                }
                else if(PageSource == "Register")
                {
                    //新注册用户不需要请求Friends和Groups
                }
            }
        }
        protected void BtnSendClick(object sender, EventArgs e)
        {

        }
    }
}