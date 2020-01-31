FROM python:3

ADD ./server/server.py /

RUN pip install flask

RUN pip install waitress

CMD env FLASK_APP=./server.py flask run --host='0.0.0.0'
