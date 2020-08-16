<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="ChatLobby.aspx.cs" Inherits="ChatClient.ChatLobby" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">

<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <title>聊天室</title>
    <style>
        form{
            color:#575454;  
            width: 767px;
            height: 650px;
            font-size:15px;
            margin-top:110px;
            margin-left: auto;
            margin-right: auto;
            margin-bottom: auto;
        }
        #TxtUserID{
            margin-left:517px;
        }
        #TxtMsgSend{
            text-align: left;
            margin-top: 3px
        }
        #LabelFriends{
           border-radius: 2px;
           border: solid 2px;
           margin-bottom: 2px; 
           color:white;
        }
        #LabelGroups{
           border-radius: 2px;
           border: solid 2px;
           margin-bottom: 2px;
           color:white;
        }
        #BtnSend{
            border-radius:2px;
            border:solid 2px;
            background-color:transparent;
            margin-left:455px;
            margin-top:1px;
            color:white;
        }
        .textbox{
            border:solid 1px;
            background:rgba(0, 0, 0, 0);
        }
        body{
            background-image: url("./BackGround/bg.jpg");
        }
    </style>
</head>
<body>
    <form id="form1" runat="server">
        <div style="margin-top:10px">
            <asp:TextBox ID="TxtUserID" runat="server" Width="237px" Height="30px" Font-Size="Large"></asp:TextBox>
        </div>
        <div style="margin-top:40px">
            <asp:TextBox ID="TxtChatStatus" runat="server" Width="233px" Height="30px" Font-Size="Large"></asp:TextBox>
        </div>
        <div style="margin-top:3px">
            <div style="width:545px; height:auto; float:left; display:inline">
                <asp:TextBox ID="TxtAllMsg" runat="server" Height="370px" Width="529px" ReadOnly="True" TextMode="MultiLine" Font-Size="Large"></asp:TextBox>
                <asp:TextBox ID="TxtMsgSend" runat="server" Height="95px" Width="529px" TextMode="MultiLine" Font-Size="Large"></asp:TextBox>
            </div>
            <div style="width:215px; height:auto; float:left; display:inline;">  
                <div style="margin:5px;">
                    <asp:Label ID="LabelFriends" runat="server" Text="好友列表" Font-Size="Large" Width="80px" BorderStyle="Solid"></asp:Label>        
                    <asp:ListBox ID="ListFriends" runat="server" Width="205px" Font-Size="Large" Height="205px"></asp:ListBox>
                </div>
                <div style="margin:5px;">
                    <asp:Label ID="LabelGroups" runat="server" Text="群组列表" Font-Size="Large" Width="80px" BorderStyle="Solid"></asp:Label>
                    <asp:ListBox ID="ListGroups" runat="server" Width="205px" Font-Size="Large" Height="205px"></asp:ListBox>
                </div>
            </div>
        </div>
        <asp:Button ID="BtnSend" runat="server" Text="发   送" Width="80px" Font-Size="Large" Height="30px" OnClick="BtnSendClick" />
    </form>
</body>
</html>
