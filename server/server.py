from flask import Flask, escape, request

app = Flask(__name__)

@app.route('/')
def hello():
    return f'Hell9o'

#if __name__ == '__main__':
#    from waitress import serve
#    serve(app, host='0.0.0.0', port=5000)

