from flask import Flask, escape, request

app = Flask(__name__)

@app.route('/')
def hello():
    return f'Hell9o'

