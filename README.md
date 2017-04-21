# libini
### A flex & bison demo of parsing the `ini` file format in C. 

Input format is to be something like this
```
#comment
[object.key]
property.key=property.value
key = escaped \
      multi-line value 
```

Sections are `Object` structures, properties a `Property` structures. Both are constructed with `Objects.new` and `Objects.property.new` respectively. 
