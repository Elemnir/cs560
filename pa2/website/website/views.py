from . import app
from .db import query_db

from flask import render_template, request

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/query")
def query():
    qlst = request.args.get('qstring', '').split()
    qsql = """SELECT p.name, e.line 
                FROM Term t, Pub p, Entry e 
                WHERE t.id = e.term AND p.id = e.pub 
                  AND t.word = '{}'
           """.format(qlst[0])

    return render_template("query.html", items=query_db(qsql))
