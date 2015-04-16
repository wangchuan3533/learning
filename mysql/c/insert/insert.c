#include <mysql.h>
#include <stdio.h>

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);        
}

int main(int argc, char **argv)
{
  MYSQL *con = mysql_init(NULL);
  int i;
  
  if (con == NULL) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }  

  if (mysql_real_connect(con, "10.91.160.167", "forumrd", "forumUHR5290", 
          "forum_adsense", 5290, NULL, 0) == NULL) 
  {
      finish_with_error(con);
  }    
  
  for (i = 0; i < 1000; i++) {
      if (mysql_query(con, "INSERT INTO stats (stats_key, stats_value, timestamp) VALUES('hello', 1, 1234) ON DUPLICATE KEY UPDATE stats_value = stats_value + 1")) {
          finish_with_error(con);
      }
      
      if (mysql_query(con, "INSERT INTO stats (stats_key, stats_value, timestamp) VALUES('hello', 1, 34) ON DUPLICATE KEY UPDATE stats_value = stats_value + 1")) {
          finish_with_error(con);
      }
      
      if (mysql_query(con, "INSERT INTO stats (stats_key, stats_value, timestamp) VALUES('hello', 1, 34) ON DUPLICATE KEY UPDATE stats_value = stats_value + 1")) {
          finish_with_error(con);
      }
      
      if (mysql_query(con, "INSERT INTO stats (stats_key, stats_value, timestamp) VALUES('hello', 1, 1234) ON DUPLICATE KEY UPDATE stats_value = stats_value + 1")) {
          finish_with_error(con);
      }
  }
  
  mysql_close(con);
  exit(0);
}
