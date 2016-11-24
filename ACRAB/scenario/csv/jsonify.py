# change encode to utf-8 with iconv etc.
import codecs,csv,json
result = str([json.dumps(l, ensure_ascii=False) for l in csv.DictReader(codecs.open('9.csv', 'rU'))])
print(result.replace('\'','"').replace('\\"','"').replace('"{','{').replace('}"','}').replace('"}','}').decode('string-escape'))

