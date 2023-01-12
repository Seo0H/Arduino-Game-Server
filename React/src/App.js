import React, { useEffect, useState, useMemo, useRef } from 'react';
import Modal from 'react-modal';
import { format } from 'date-fns';

import axios from 'axios';
import ".//css/styles.css";
import ".//css/modal.css";

import Table from "./Table";
import Side_Headers from './Side-Header';
import InsertName from './InsertName';

//검색용 export
// const [exColumns, setExColumns] = useState();
// const [exData, setExData] = useState();

function App() {

  //받아오는 변수
  const [inputData, setInputData] = useState({
    data: [{
      id: '',
      name: '',
      date: '',
      time: ''
    },]
  });

  //사용하는 변수
  const [useData, setuseData] = useState({
    data: [{
      id: '',
      name: '',
      date: '',
      time: ''
    },]
  });

  const [modalIsOpen, setModalIsOpen] = useState(false);
  const [nameInput, setNameInput] = useState('');
  const [id, setId] = useState();
  const count = 0;

  //처음 받아오는 부분 inputdata, usedata 설정
  useEffect(()=>{
      loadData();
      setuseData(inputData);
  })

  //usedata != inputdata / usedata = inputdata 로 바꾸는 부분
 // let previousVal = usePrevious(useData);
  useEffect(() => {
    if (inputData !== undefined) {
      //데이터 변화 감지
      //전 후 데이터가 같다면->pass
      //다르다면 데이터 리로드
      let inputSort = Object.entries(inputData).sort()
      let useSort = Object.entries(useData).sort()
      // console.log("inputSort"+JSON.stringify(inputSort));
      // console.log("useSort"+JSON.stringify(useSort));
      // console.log(JSON.stringify(useSort) == JSON.stringify(inputSort));
      if (JSON.stringify(useData) == JSON.stringify(inputData)) {
        console.log("데이터 같음");
      }else{
        console.log("데이터 다름. 리로드");
        loadData();
        setModalIsOpen(true);
        // console.log(inputData.data.id);
        setId(useData.data.id);
      }
    }
  }, [inputData])

  // -------------data 불러오기---------------------
  async function loadData(){
    try {
      const res = axios.get('/api/info');
      const resData = (await res).data;
      setInputData(resData);
    } catch (e) {
      console.error(e.message);
    }
  }

  // ------------ data 비교 --------

  function usePrevious(value) {
    const ref = useRef();
    useEffect(() => {
      ref.current = value;
    });
    return ref.current;
  }

  //---------------------------modal--------------------------
  function getId() {
    let index = [];
    inputData.data.map((id, i) => {
      index.push(inputData.data[i].id);
    });

    index.sort()
    let id = index[index.length - 1];

    return id;
  }

  function updateName(e) {

    e.preventDefault();
    let id = getId();

    //가장 최근 들어온 id 값 추출
    if (id !== undefined) {
      axios.post('/api/add', {
        "name": nameInput,
        "id": id,
      })
        .then((res) => {
          console.log(res);
          return res.data;
        })
        .catch(e => {
          console.log(e.message);
        });
    }
    setModalIsOpen(false);
  }

  function time() {
    let id = getId();
    let data = inputData.data.filter((n) => n.id === id);
    return data[0].time;
  }

  // -------------------------table--------------------------

  const columns = useMemo(
    () => [
      {
        accessor: "ranking",
        Header: "Ranking"
      },
      {
        accessor: "name",
        Header: "Nickname"
      },
      {
        accessor: "time",
        Header: "Score"
      },
      {
        accessor: "date",
        Header: "Date"
      },
    ],
    []
  );

  const data = useMemo(
    () =>
      inputData.data.map((el, i) => ({
        ranking: i + 1,
        name: el.name,
        time: el.time,
        date: el.date,
      })),
    [inputData]
  );

  // setExColumns(columns);
  // setExData(data);

  return (
    <div className="root-container">
      <div className="body-container">
        <div className='side-header'>
          <Side_Headers
            columns={columns}
            data={data}
            className="table"
            />
        </div>
        <div className='table-background'>
          <img className='joystick' src='/joystick.png' />
          <img className='led' src='/led.png' />
          <Table
            columns={columns}
            data={data}
            className="table"
          />
        </div>
      </div>
      <div>
        <button onClick={() => setModalIsOpen(true)}>Modal Open</button>
        <Modal isOpen={modalIsOpen} onRequestClose={() => setModalIsOpen(false)} className="Modal" >
          <div className="modalBody">
            <div className='recode containor'>
              <a className='modal-text'>Recode</a>
              <input
                className='recode-input'
                type="text"
                value={time()}
                disabled />
            </div>
            <div className='Nickname containor'>
              <a className='modal-text'>Nickname</a>
              <input
                autoFocus
                className='nickname-input'
                type="text"
                value={nameInput}
                onChange={({ target: { value } }) => setNameInput(value)}
              />
            </div>
          </div>
          <button className='regist-btn' onClick={updateName} > 등록 </button>
        </Modal>
      </div>

    </div>
  );
}

export default App;
