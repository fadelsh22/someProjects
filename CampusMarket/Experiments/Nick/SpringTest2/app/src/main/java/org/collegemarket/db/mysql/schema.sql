CREATE TABLE IF NOT EXISTS users(
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    user_name  VARCHAR(20),
    password   VARCHAR(30),
    first_name VARCHAR(30),
    last_name  VARCHAR(30)
);