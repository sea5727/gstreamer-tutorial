#!/bin/bash

python webrtc_server.py & FLASK_APP=app flask run --host=0.0.0.0 --port=5000 --cert=cert.pem --key=key.pem 