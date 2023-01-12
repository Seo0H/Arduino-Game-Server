const db = require('../db');
const express = require('express');
const app = express();
const bodyParser = require('body-parser');

app.use(bodyParser.json());

app.get('/time', (req, res) => {
    db.query('SELECT time FROM record', (err, data) => {
        if (!err) res.send({ time: data });
        else res.send(err);
    })
})

app.get('/', (req, res) => {
    res.send('Server Response Success');
});

app.get('/hello', (req, res) => {
    res.send({ hello: 'Hello react' });
});

let preData;
app.get('/api/info', (req, res) => {
    db.connect(function (err) {
        //console.log('Cunnecte');
        db.query('SELECT * FROM record ORDER BY time DESC', (err, data) => {
            if (!err) {
                res.send({ data: data });
                preData = data;
            } else {
                res.send(err);
            };
        });
    });
});

app.post('/api/add', (req, res) => {
    console.log(req.body);

    db.connect(function (err) {
        console.log('Cunnecte');
        db.query(`UPDATE record set name= '${req.body.name}' where id = '${req.body.id}'`);
    });
});

module.exports = app;