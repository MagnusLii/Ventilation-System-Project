DB layout:<br><br>

Table readings {<br>
  index BIGINT [pk, NOT NULL, UNIQUE, increment]<br>
  speed INT<br>
  pressure INT<br>
  co2 INT<br>
  ah INT<br>
  rh INT<br>
  temp INT<br>
  timestamp DATETIME<br>
}<br>
<br>
Table logmessages {<br>
  index BIGINT [pk, NOT NULL, UNIQUE, increment]<br>
  logcode text<br>
  timestamp DATETIME<br>
}