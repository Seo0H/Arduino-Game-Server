
const cors = require("cors");
const express = require('express');
const app = express();

// api 처리는 './routes/index'에서 일괄처리
const api = require('./routes/index');

// var corsOption = {
//     Origin: "http://localhost:3000"
// };
// app.use(cors(corsOption));
app.use('/', api);


// server port 4000 할당
// 클라이언트와 다른 번호로 충돌나지 않도록
const PORT = 4000;
app.listen(PORT, () => {
    console.log(`서버 실행중 : http://localhost:${PORT}/`)
})
