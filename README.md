# Stats

`stats` is a CLI tool to compute basic statistics on a list of numbers.  
This is a project for me to learn C++, so it is probably not the most efficient, 
but it runs fast enough and should be usable by anyone. 

## Usage

`stats` is meant to be used on standard input only, I might add the option to read from a file later.   
`stats` expects a list of numbers, one per line, as input. It is meant to be used in conjunction with other
tools such as `cut` to parse more complex files such as `.csv`. 

### Basic examples
```shell
# Create list of numbers
$ echo "1\n2\n3\n4\n5\n6\n7\n8\n9\n10" > test.data

# Get basic statistics on list of numbers 
$ cat test.data | stats

Mean:     5.5
Variance: 8.25 (2.87228 SD)
Min:      1
Max:      10
Count:    10

# Format output as json
$ cat test.data | stats --json

{
	"mean": 5.5,
	"variance": 8.25,
	"stdev": 2.87228,
	"min": 1,
	"max": 10,
	"count": 10
}

# Compute median and quartiles
$ cat test.data | stats --quartiles

Mean:     5.5
Variance: 8.25 (2.87228 SD)
Min:      1
Max:      10
Count:    10
Q1:       4
Median:   6
Q3:       9

# Download csv file
$ curl "https://gist.githubusercontent.com/netj/8836201/raw/6f9306ad21398ea43cba4f7d537619d0e07d5ae3/iris.csv" > iris.csv

# Get statistics on petal length (3rd column)
$ cut -d "," -f 3 iris.csv | stats --skip-header

Mean:     3.758
Variance: 3.0955 (1.7594 SD)
Min:      1
Max:      6.9
Count:    150
Q1:       1.6
Median:   4.4
Q3:       5.1
```

### Options

- `-j`, `--json`: Output the statistics in JSON format if you want to use them in a subsequent program
- `-q`, `--quartiles`: Compute the median, Q1 and Q3 quartiles. *(This is not enabled by default because computing 
these values implies storing all values in memory which can be a problem with very large inputs)* 
- `-s`, `--skip-header`: Ignores the first line of the input. *(Useful when parsing from csv files with headers)* 
- `-h`, `--help`: Show help message