SET profiling = 1;
SET max_heap_table_size = 1024*1024*1024*3;

DROP TABLE IF EXISTS posts;
CREATE TABLE posts (Body INT, ViewCount INT, LastActivityDate INT, Title INT, LastEditorUserId INT, LastEditorDisplayName INT, LastEditDate INT, CommentCount INT, AnswerCount INT, AcceptedAnswerId INT, Score INT, PostTypeId INT, OwnerUserId INT, Tags INT, CreationDate INT, FavoriteCount INT, Id INT, ParentId INT, CommunityOwnedDate INT, ClosedDate INT) ENGINE = MEMORY;
LOAD DATA LOCAL INFILE '/lfs/local/0/ringo/testfiles/posts_SIZEPARAM.hashed.tsv' INTO TABLE posts IGNORE 1 LINES;
ALTER TABLE posts ADD INDEX (PostTypeId), ADD INDEX (OwnerUserId), ADD INDEX (Id);
SELECT SQL_CALC_FOUND_ROWS *
FROM posts T1, posts T2
WHERE T1.PostTypeId = 2 AND T1.ParentId = T2.id
GROUP BY T1.OwnerUserId, T2.OwnerUserId
LIMIT 0;
SELECT FOUND_ROWS();
    
DROP TABLE IF EXISTS comments;
CREATE TABLE comments (CreationDate INT, Body INT, UserId INT, INDEX USING HASH (UserId), Score INT, PostId INT, INDEX USING HASH (PostId), Id INT) ENGINE = MEMORY;
LOAD DATA LOCAL INFILE '/lfs/local/0/ringo/testfiles/comments_SIZEPARAM.hashed.tsv' INTO TABLE comments IGNORE 1 LINES;
ALTER TABLE comments ADD INDEX (UserId), ADD INDEX (PostId);
SELECT SQL_CALC_FOUND_ROWS *
FROM comments T1, comments T2
WHERE T1.PostId = T2.PostId AND T1.UserId != T2.UserId
GROUP BY T1.UserId, T2.UserId
LIMIT 0;
SELECT FOUND_ROWS();

SHOW PROFILES;
