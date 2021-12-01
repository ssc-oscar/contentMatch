# LSH-type ways to approximately match content

## Goal

Enhance WoC indexing beyond matching sha1 and sha1 of the ctags with winnowing [1]. 

Winnowing is a flavor of locality sensitive hashing (LSH) and probably can be improved, but it may 
still be a good start to identify matches of parts of the file.

## Performance

Winnowing encoding takes 1M blobs/min single threaded. 

For 15B blobs thats only 10 calendar days on a single thread, so relatively insignificant amount of computation on 
a cluster.

Search by matched fingerprints: the approach is to ignore fingerprints that match too many blobs/projects

## Implementation

osscan has an implementation of winnowing in C, that can probably be reused.



## Rrferences

[1] Winnowing: local algorithms for document fingerprinting
SIGMOD '03: Proceedings of the 2003 ACM SIGMOD international conference on Management of dataJune 2003 Pages 76–85https://doi.org/10.1145/872757.872770

