from flask import Flask, send_from_directory
import ssl

app = Flask(__name__)

@app.route('/')
def home():
    print('home??')
    return app.send_static_file('index.html')


if __name__ == '__main__':
    print('name??')
    ssl_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS)
    ssl_ctx.load_cert_chain(certfile='./cert.pem', keyfile='./key.pem')

    app.run(debug=True, host='0.0.0.0', port=5005, ssl_context=ssl_ctx)