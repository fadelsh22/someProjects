<%--
  - Author(s): Fadel Alshammasi & Farouk AL Obaidi
  - Date: 4/16/19
  - Copyright Notice:
  - @(#)
  --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
	pageEncoding="ISO-8859-1"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<%@ page import="java.io.*,java.util.*,java.sql.*"%>
<%@ page import="javax.servlet.http.*,javax.servlet.*"%>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title>Tweet Query Results</title>
</head>
<body style = "background-color:powderblue;">
	<!-- any error is redirected to ShowError.jsp -->
	<%-- <%@ page errorPage="ShowError.jsp"%> --%>
	<!-- include all the database connection configurations -->
	<%
		Connection conn=null;
		ResultSet rs=null;
		String DB_URL = "jdbc:mysql://cs363winservdb:3306/group44?useSSL=false";

		Class.forName("com.mysql.jdbc.Driver");
		conn = DriverManager.getConnection(DB_URL, (String)session.getAttribute("username"), (String)session.getAttribute("password"));

		//String findReachable = request.getParameter("node_selector");
		CallableStatement cstmt;
		out.println("<h3>Here's the Query results for " + request.getParameter("Query") + ":</h3>");

		if (request.getParameter("Query").equals("Q1")) {
			cstmt = conn.prepareCall("{call Q1(?,?,?)}");
			cstmt.setString(1, request.getParameter("K"));
			cstmt.setString(2, request.getParameter("Month"));
			cstmt.setString(3, request.getParameter("Year"));
			rs = cstmt.executeQuery();
			while (rs.next()) {
            int retweet_count = rs.getInt("tweet.retweet_count");
            String tweet_text = rs.getString("tweet.tweet_text");
            String uscreen_name = rs.getString("tweet.uscreen_name");
            String category = rs.getString("Users.category");
            String sub_category = rs.getString("Users.sub_category");
            out.println("<h3>" + retweet_count +
                               "\t" + tweet_text + "\t" + uscreen_name +
                               "\t" + category + "\t"+ sub_category + "</h3>");
			}
			rs.close();
			conn.close();

		} else if (request.getParameter("Query").equals("Q2")) {
			cstmt = conn.prepareCall("{call Q2(?,?,?,?)}");
			cstmt.setString(1, request.getParameter("K"));
			cstmt.setString(2, request.getParameter("Hashtag"));
			cstmt.setString(3, request.getParameter("Month"));
			cstmt.setString(4, request.getParameter("Year"));
			rs = cstmt.executeQuery();
			while (rs.next()) {
            String screen_name = rs.getString("Users.screen_name");
            String category = rs.getString("Users.category");
					  String tweet_text = rs.getString("Tweet.tweet_text");
						int retweet_count = rs.getInt("Tweet.retweet_count");
            out.println("<h3>" + screen_name +
                               "\t" + category + "\t" + tweet_text +
                               "\t" + retweet_count + "</h3>");
			}
			rs.close();
			conn.close();

			//TODO
		} else if (request.getParameter("Query").equals("Q3")) {
			cstmt = conn.prepareCall("{call Q3(?,?)}");
			cstmt.setString(1, request.getParameter("K"));
			cstmt.setString(2, request.getParameter("Year"));
			rs = cstmt.executeQuery();
			while (rs.next()) {
            String stateCount = rs.getString("state_count"); // do the count
						String stateList = rs.getString("state_concat");
					  String hashtag = rs.getString("hashtag.hashtagname");
            out.println("<h3>" + "\t" + stateCount + "\t" +
												stateList +  "\t" + hashtag + "</h3>");

			}
			rs.close();
			conn.close();

			//TODO
			} else if (request.getParameter("Query").equals("Q6")) {
			cstmt = conn.prepareCall("{call Q6(?,?)}");
			cstmt.setString(1, request.getParameter("K"));
			cstmt.setString(2, request.getParameter("HashtagList"));
			rs = cstmt.executeQuery();
			while (rs.next()) {
            String screen_name = rs.getString("Users.screen_name");
            String state = rs.getString("Users.state");
            out.println("<h3>" + screen_name +
                               "\t" + state + "\t" + "</h3>");
			}
			rs.close();
			conn.close();

			//TODO
		} else if (request.getParameter("Query").equals("Q10")) {
		cstmt = conn.prepareCall("{call Q10(?,?,?)}");
		cstmt.setString(1, request.getParameter("stateList"));
		cstmt.setString(2, request.getParameter("Month"));
		cstmt.setString(3, request.getParameter("Year"));
		rs = cstmt.executeQuery();
		while (rs.next()) {
					String hashtag = rs.getString("hashtag.hashtagname");
					String stateList = rs.getString("stateList");
					out.println("<h3>" + hashtag +
														 "\t" + stateList + "\t" + "</h3>");
		}
		rs.close();
		conn.close();

		//TODO
	} else if (request.getParameter("Query").equals("Q15")) {
		cstmt = conn.prepareCall("{call Q15(?,?,?)}");
		cstmt.setString(1, request.getParameter("Sub_Category"));
		cstmt.setString(2, request.getParameter("Month"));
		cstmt.setString(3, request.getParameter("Year"));
		rs = cstmt.executeQuery();
		while (rs.next()) {
					String screen_name = rs.getString("users.screen_name");
					String state = rs.getString("users.state");
					String Url = rs.getString("url_concat");
					out.println("<h3>" + screen_name +
														 "\t" + state + "\t" +
														 Url + "\t" + "</h3>");
		}
		rs.close();
		conn.close();

		} else if (request.getParameter("Query").equals("Q23")) {
		cstmt = conn.prepareCall("{call Q23(?,?,?,?)}");
		cstmt.setString(1, request.getParameter("Sub_Category"));
		cstmt.setString(2, request.getParameter("MonthList"));
		cstmt.setString(3, request.getParameter("Year"));
		cstmt.setString(4, request.getParameter("K"));
		rs = cstmt.executeQuery();
		while (rs.next()) {
          String hashtag = rs.getString("Hashtag.hashtagname");
          int hastagCount = rs.getInt("hashtag_count");
          out.println("<h3>" + hashtag +
                             "\t " + hastagCount + "\t" + "</h3>");
		}
		rs.close();
		conn.close();

		//TODO
	} else if (request.getParameter("Query").equals("Q27")) {
	cstmt = conn.prepareCall("{call Q27(?,?,?,?)}");
	cstmt.setString(1, request.getParameter("Month"));
	cstmt.setString(2, request.getParameter("MonthB"));
	cstmt.setString(3, request.getParameter("Year"));
	cstmt.setString(4, request.getParameter("K"));
	rs = cstmt.executeQuery();
	while (rs.next()) {
				String screen_name = rs.getString("users.screen_name");
				out.println("<h3>" + screen_name +
											"\t" + "</h3>");
	}
	rs.close();
	conn.close();

		//TODO
	} else if (request.getParameter("Query").equals("INSERT")) {
	cstmt = conn.prepareCall("{call QI(?,?,?,?,?,?,?)}");
	cstmt.setString(1, request.getParameter("QIScreen"));
	cstmt.setString(2, request.getParameter("QIUname"));
	cstmt.setString(3, request.getParameter("QIsubcat"));
	cstmt.setString(4, request.getParameter("QIcat"));
	cstmt.setString(5, request.getParameter("QIState"));
	cstmt.setString(6, request.getParameter("QIFollowers"));
	cstmt.setString(7, request.getParameter("QIFollowing"));
	rs = cstmt.executeQuery();
	//TODO HANDLE ERROR DUPLICATE
	out.println("<h3>SUCCESS</h3>");

	rs.close();
	conn.close();

	} else if (request.getParameter("Query").equals("DELETE")) {
	cstmt = conn.prepareCall("{call QD(?)}");
	cstmt.setString(1, request.getParameter("QIScreenDelete"));
	rs = cstmt.executeQuery();
	out.println("<h3>SUCCESS</h3>");

	rs.close();
	conn.close();

	}else {
		out.println("<h3> Error Please Navigate Back </h3>");
	}
	%>


</body>
</html>
