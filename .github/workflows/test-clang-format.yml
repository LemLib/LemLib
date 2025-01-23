name: test-clang-format

on:
  push:
    branches:
      - '*'
  pull_request:


jobs:
  build:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v2
    - uses: d-griet/clang-format-lint-action@99a106be2f3f1a92d9783ea7c744fde62d8ce1fa
      with:
        source: './src/lemlib ./include/lemlib'
        extensions: 'hpp,cpp'
        clangFormatVersion: 19
