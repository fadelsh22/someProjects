<%--
  - Author(s): Fadel Alshammasi & Farouk AL Obaidi
  - Date: 4/16/19
  - Copyright Notice:
  - @(#)
  --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
	pageEncoding="ISO-8859-1"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<%@ page import="java.io.*,java.util.*,java.sql.*"%>
<%@ page import="javax.servlet.http.*,javax.servlet.*"%>
<html>
<head>
<title>Tweet Query Options</title>
</head>
<body style = "background-color:powderblue;">
	<%
		Connection conn =null;
		Statement stmt =null;
		ResultSet rs =null;
		String DB_URL = "jdbc:mysql://cs363winservdb:3306/group44?useSSL=false";

		//HttpSession session = request.getSession();

			session.setAttribute("username","farook");
			session.setAttribute("password","363s288213");

		// Java way for handling an error using try catch
		try {
			Class.forName("com.mysql.jdbc.Driver");
			conn = DriverManager.getConnection(DB_URL, (String)session.getAttribute("username"), (String)session.getAttribute("password"));

			CallableStatement cstmt = conn.prepareCall("{call Qlogin(?,?)}");

			cstmt.setString(1, request.getParameter("username"));
			cstmt.setString(2, request.getParameter("password"));

			rs = cstmt.executeQuery();

			if(rs.next()){
				String	username = rs.getString("login.username");
				String	password = rs.getString("login.pass");
				String role = rs.getString("login.role");
				if( role.equals("readOnly")){
					%>
					<form method="post" action="results.jsp" style="text-align:center;">
					<h1>Select the Query you want to excute and fill the required information:</h1>


						Query:<br>
						<select name="Query">
								<option value="Q1">Q1</option>
								<option value="Q2">Q2</option>
								<option value="Q3">Q3</option>
								<option value="Q6">Q6</option>
								<option value="Q10">Q10</option>
								<option value="Q15">Q15</option>
								<option value="Q23">Q23</option>
								<option value="Q27">Q27</option>
						</select>

						<br>
						K:<br>
						<input name="K" type="text" placeholder="Valid for: Q1, Q2, Q3, Q6, Q23 or Q27" style= "width:430px;">
						<br>

						Month:<br>
						<input name="Month" type="text" placeholder="Valid for: Q1, Q2, Q10, Q15 or Q27" style= "width:430px;">
						<br>

						MonthB:<br>
						<input name="MonthB" type="text" placeholder="Valid for: Q27" style= "width:430px;">
						<br>

						Month List:<br>
						<input name="MonthList" type="text" placeholder="Valid for: Q23" style= "width:430px;">
						<br>

						Year:<br>
						<input name="Year" type="text" placeholder="Valid for: Q1, Q2, Q3, Q10, Q15, Q23 or Q27" style= "width:430px;">
						<br>

						Hashtag:<br>
						<input name="Hashtag" type="text" placeholder="Valid for: Q2" style= "width:430px;">
						<br>

						Hashtag List:<br>
						<input name="HashtagList" type="text" placeholder="Valid for: Q6" style= "width:430px;">
						<br>

						State List:<br>
						<input name="stateList" type="text" placeholder="Valid for: Q10" style= "width:430px;">
						<br>

						Sub_Category:<br>
						<input name="Sub_Category" type="text" placeholder="Valid for: Q15 or Q23" style= "width:430px;">
						<br>
						<br>

						<input type="submit" value="GO">
					</form>
						<br>
						<%
				}else{
					%>
					<form method="post" action="results.jsp" style="text-align:center;">
					<h1>Select the Query you want to excute and fill the required information:</h1>


						Query:<br>
						<select name="Query">
								<option value="INSERT">INSERT</option>
								<option value="DELETE">DELETE</option>
								<option value="Q1">Q1</option>
								<option value="Q2">Q2</option>
								<option value="Q3">Q3</option>
								<option value="Q6">Q6</option>
								<option value="Q10">Q10</option>
								<option value="Q15">Q15</option>
								<option value="Q23">Q23</option>
								<option value="Q27">Q27</option>
						</select>
						<br>

						<br>
						K:<br>
						<input name="K" type="text" placeholder="Valid for: Q1, Q2, Q3, Q6, Q23 or Q27" style= "width:430px;">
						<br>

						Month:<br>
						<input name="Month" type="text" placeholder="Valid for: Q1, Q2, Q10, Q15 or Q27" style= "width:430px;">
						<br>

						MonthB:<br>
						<input name="MonthB" type="text" placeholder="Valid for: Q27" style= "width:430px;">
						<br>

						Month List:<br>
						<input name="MonthList" type="text" placeholder="Valid for: Q23" style= "width:430px;">
						<br>

						Year:<br>
						<input name="Year" type="text" placeholder="Valid for: Q1, Q2, Q3, Q10, Q15, Q23 or Q27" style= "width:430px;">
						<br>

						Hashtag:<br>
						<input name="Hashtag" type="text" placeholder="Valid for: Q2" style= "width:430px;">
						<br>

						Hashtag List:<br>
						<input name="HashtagList" type="text" placeholder="Valid for: Q6" style= "width:430px;">
						<br>

						State List:<br>
						<input name="stateList" type="text" placeholder="Valid for: Q10" style= "width:430px;">
						<br>

						Sub_Category:<br>
						<input name="Sub_Category" type="text" placeholder="Valid for: Q15 or Q23" style= "width:430px;">
						<br>
						<br>

						Insert New User:<br>
						<input name="QIScreen" type="text" placeholder="screen name" style= "width:300px;">
						<br>
						<input name="QIUname" type="text" placeholder="username" style= "width:300px;">
						<br>
						<input name="QIsubcat" type="text" placeholder="sub_category" style= "width:300px;">
						<br>
						<input name="QIcat" type="text" placeholder="category" style= "width:300px;">
						<br>
						<input name="QIState" type="text" placeholder="state" style= "width:300px;">
						<br>
						<input name="QIFollowers" type="text" placeholder="followers" style= "width:300px;">
						<br>
						<input name="QIFollowing" type="text" placeholder="following" style= "width:300px;">
						<br>
						<br>

						Delete Existing User:<br>
						<input name="QIScreenDelete" type="text" placeholder="screen name" style= "width:300px;">
						<br>
						<br>

						<input type="submit" value="GO">
					</form>
				<%
				}
		}else{
			String site = new String("Index.html");
			response.setStatus(response.SC_MOVED_TEMPORARILY);
			response.setHeader("Location", site);
		}

		} catch (SQLException e) {
			out.println("An exception occurred: " + e.getMessage());
		} finally {
			if (rs!= null) rs.close();
			if (stmt!= null) stmt.close();
			if (conn != null) conn.close();
		}
	%>


</body>
</html>
