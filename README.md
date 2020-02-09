# HashSmash
<h1>Create and attempt to break md5 hashes<h1>
<p>The goal of this project was for me to play around with md5 hashes</p>

<h2>Phase 1</h2>
<p>The "ToHash" project was for me to become familiar with the md5 algorithm and understand the specifics of how it works. This project is now functional. Note that it currently can only hash strings up to 55 characters. The output is in binary.</p>

<h2>Phase 2</h2>
<p>The "ReverseHash" project is my attempt to break md5 hashes.</p>
<ul>
<li>After each 64 operation round, the result of the round is added to the values currently in the buffers. Assuming the message is 55 characters of less, resulting in only one round, subtracting the known starting values from the buffers will give their states at the end of the round.</li>
<li>For each operation the inverse of creating the hash can be done. Subtracting the value of buffer B, undoing the bit rotation, subtracting the known constant, and subtracting the result of applying a know function to the other buffers leaves only the value of buffer A plus a message chunk.</li>
<li>Assuming that all characters are letters, numbers, and symbols represented by ASCII, message chuncks will have zeros in known locations. This will give some direction in guessing the values for the message chunk and buffer A.</li>
<li>My goal is that using this methodology will allow me to reverse an md5 hash to at least some degree.</li>
</ul>