﻿<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Login.aspx.cs" Inherits="ChatClient.Login" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <title>登陆</title>
    <style>
        form{
            color:#575454;  
            width:500px;
            margin: auto;
            font-size:15px;
            margin-top:260px;
	    }
        #spanpsd{
            margin-left:125px;
            color:white;
        }
        #spanuser{
            margin-left:110px;
            color:white;
        }
        div{
            margin:30px auto;
            align-content:center;
        } 
        .textbox{
            border:solid 1px;
            background:rgba(0, 0, 0, 0);
        }
        #LinkBtnToRegister{
            text-decoration:none;
            margin-left:230px; 
        }
        #BtnLogin{
            border-radius:2px;
            border:solid 1px;
            background-color:transparent;
            margin-left:150px;
            margin-top:10px;
            color:white;
        }
        body{
            background-image: url("./BackGround/bg.jpg");
        }
    </style>
</head>
<body>
    <form id="form1" runat="server">
        <div>
            <div>
                <span id="spanuser">用户ID:</span>
                <asp:TextBox ID="UserName" runat="server" CssClass="textbox" Height="30px" Width="240px"></asp:TextBox>
            </div>

            <div>
                <span id="spanpsd">密码:</span>
                <asp:TextBox ID="Password" runat="server" CssClass="textbox" Height="30px" Width="240px" TextMode="Password"></asp:TextBox>
            </div>

            <div>
                <asp:LinkButton ID="LinkBtnToRegister" runat="server" OnClick="LinkBtnToRegisterClick">没有账号?注册</asp:LinkButton>
                <br />
                <asp:Button ID="BtnLogin" runat="server" Text="登 录" Width="270px" Height="40px" OnClick="BtnLoginClick" />
            </div>
        </div>
    </form>
</body>
</html>
