const mysql  = require('mysql');

const db  = mysql.createConnection({
    host : '*',
    user : 'root',
    port: 3307,
    password : '1234',
    database : 'game',
    dateStrings: 'date',
});


db.connect((err) => {
    if(err){
        console.log('error when connecting to db:', err);
        setTimeout(handleDisconnect, 2000);
    }

    console.log('success');
});

db.on('error', function(err) {
    console.log('db error', err);
    if(err.code === 'PROTOCOL_CONNECTION_LOST') {
      return handleDisconnect();
    } else {
      throw err;
    }
  });

module.exports = db ;