#!/bin/bash
echo test `check`
cat data_reversi_check_black.txt | ./reversi check white
cat data_reversi_check_black.txt | ./reversi check black

echo test `parse`
cat data_reversi_parse.txt | ./reversi parse black
cat data_reversi_parse.txt | ./reversi parse white

cat data_reversi_parse_cantput.txt | ./reversi parse black
cat data_reversi_parse_cantput.txt | ./reversi parse white

cat data_reversi_black_cantput.txt | ./reversi parse black
cat data_reversi_black_cantput.txt | ./reversi parse white
