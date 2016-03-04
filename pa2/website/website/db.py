from website import app

from flask import _app_ctx_stack
from sqlite3 import dbapi2 as sqlite3

def get_db():
    top = _app_ctx_stack.top
    if not hasattr(top, 'sqlite_db'):
        top.sqlite_db = sqlite3.connect('test.db')
        top.sqlite_db.row_factory = sqlite3.Row
    return top.sqlite_db

@app.teardown_appcontext
def close_database(exception):
    top = _app_ctx_stack.top
    if hasattr(top, 'sqlite_db'):
        top.sqlite_db.close()

@app.cli.command()
def initdb():
    db = get_db()
    with app.open_resource('schema.sql', mode='r') as f:
        db.cursor().executescript(f.read())
    db.commit()

def query_db(query, args=(), one=False):
    cur = get_db().execute(query, args)
    rv = cur.fetchall()
    return (rv[0] if rv else None) if one else rv
