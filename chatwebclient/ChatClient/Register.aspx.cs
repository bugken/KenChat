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
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace ChatClient
{
    public class RegisterInfo
    {
        public int msgid { get; set; }
        public string name { get; set; }
        public string password { get; set; }
    }
    class RegisterReplyInfo
    {
        public int errno { get; set; }
        public string errmsg { get; set; }
        public int msgid { get; set; }
        public int id { get; set; }
    }
    public partial class Register : System.Web.UI.Page
    {
        private bool UserNameIselgal = false;
        private bool PsdIselgal = false;
        private bool CanRegister = false;
        private string URLs = "http://192.168.107.128:8000";
        private JavaScriptSerializer JsonParserJss;
        private RegisterInfo resisterInfo;

        public Register()
        {
            System.Diagnostics.Debug.WriteLine("Register");
            resisterInfo = new RegisterInfo();
            resisterInfo.msgid = 3;

            JsonParserJss = new JavaScriptSerializer();
        }
        protected void Page_Load(object sender, EventArgs e)
        {

        }
        protected void BtnLinkToLoginClick(object sender, EventArgs e)
        {
            Response.Redirect("Login.aspx", false);
        }
        private bool RegisterByWebClient(ref string DataBuff)
        {
            System.Diagnostics.Debug.WriteLine("RegisterByWebClient");

            bool Result = true;
            WebClient client = new WebClient();
            byte[] jsonData = Encoding.UTF8.GetBytes(JsonParserJss.Serialize(resisterInfo));
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
        protected void BtnRegisterClick(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("BtnRegisterClick");
            resisterInfo.name = rUserNameText.Text;
            resisterInfo.password = rPsdText.Text;

            string RegisterBuff = "";
            if (RegisterByWebClient(ref RegisterBuff))
            {
                System.Diagnostics.Debug.WriteLine(RegisterBuff);
                //RegisterReplyInfo registerReplyData = JsonParserJss.Deserialize<RegisterReplyInfo>(RegisterBuff);
                RegisterReplyInfo registerReplyData = JsonConvert.DeserializeObject<RegisterReplyInfo>(RegisterBuff);
                if (registerReplyData.errno == 0)
                {
                    string ID = registerReplyData.id.ToString();
                    Response.Redirect("ChatLobby.aspx?PageFrom=Register&UserID=" + ID
                        + "&UserName=" + resisterInfo.name, false);
                }
                else
                {
                    Response.Write("<script>alert('" + registerReplyData.errmsg + "')</script>");
                }
            }
            else 
            {
                Response.Write("BtnRegisterClick Failed");
            }
        }
        protected void CustomValidator1_ServerValidate(object source, System.Web.UI.WebControls.ServerValidateEventArgs args)
        {
            if (rUserNameText.Text.Equals("用户名"))
            {
                CustomValidator1.ErrorMessage = "*用户名为空";
                args.IsValid = false;
            }
            else if (System.Text.RegularExpressions.Regex.IsMatch(rUserNameText.Text, "^[0-9a-zA-Z]+$") &&
                  rUserNameText.Text.Length > 5 && rUserNameText.Text.Length < 11)
            {
                args.IsValid = true;
                UserNameIselgal = true;
            }
            else
            {
                CustomValidator1.ErrorMessage = "*用户名由6~10位数字和字母构成";
                args.IsValid = false;
            }
        }
        protected void CustomValidator2_ServerValidate(object source, System.Web.UI.WebControls.ServerValidateEventArgs args)
        {
            if (rPsdText.Text.Equals("密码"))
            {
                CustomValidator2.ErrorMessage = "*密码为空";
                args.IsValid = false;
            }
            else if (System.Text.RegularExpressions.Regex.IsMatch(rPsdText.Text, "^[0-9a-zA-Z]+$") &&
              rPsdText.Text.Length > 4)
            {
                args.IsValid = true;
            }
            else
            {
                CustomValidator2.ErrorMessage = "*密码由全数字和字母构成且不少于5位";
                args.IsValid = false;
            }
        }
        protected void CustomValidator3_ServerValidate(object source, System.Web.UI.WebControls.ServerValidateEventArgs args)
        {
            if (rrPsdText.Text.Equals("") || rrPsdText.Text.Equals("确认密码"))
            {
                args.IsValid = false;
                CustomValidator3.ErrorMessage = "*确认密码为空";
            }
            else if (!rrPsdText.Text.Equals(rPsdText.Text))
            {
                args.IsValid = false;
                CustomValidator3.ErrorMessage = "*两次密码不一致";
            }
            else
            {
                PsdIselgal = true;
                args.IsValid = true;
            }
        }
    }
}