# HashSmash
Create and attempt to break md5 hashes

The goal of this was for me to play around with md5. I am currently working on hashing a message with md5 in order to fully understand how it works. Once I have a good understanding, I plan to start working on ways to break it.

Important notes:
The result is currently output with each 32 bit chunk in little endian.
The result is not yet completely correct. It is somewhat close, but some calculations are currently losing some accuracy.