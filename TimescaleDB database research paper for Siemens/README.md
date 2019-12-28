
## TimescaleDB Research
One of the projects that I worked on at Siemens (that I'm able to share) was investagating a new time-series database (TimescaleDB) and evaluating its performance as it could be used in some small installations of Siemens' internal software. The weather data used for evaluation has over a million records.The batch intersrting method that I used(a Go script) was about 34 times faster than PostgreSQL's built-in COPY!


