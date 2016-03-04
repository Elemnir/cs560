from . import app
from .db import query_db

from flask import render_template

@app.route("/")
def index():
    return render_template("index.html")
