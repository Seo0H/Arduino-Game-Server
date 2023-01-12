import Modal from 'react-modal';
import React from 'react';
import { useState, useEffect } from 'react';
import axios from 'axios';
import ".//css/modal.css";


function InsertName(props) {

    //id값도 같이 가야 하는데 .....
    //이전 데이터와 현재 데이터 비교해서 가장 최근에 들어온 데이터의 id만 username 변경 가능하게 하기

    const [nameInput, setNameInput] = useState('');

    function updateName(e) {

        console.log(nameInput);
        e.preventDefault();

        axios.post('/api/add', {
            data: nameInput,
        })
            .then((res) => {
                console.log(res);
            })
            .catch(e => {
                console.log(e.message);
            });
    }

    useEffect(() => {
        const setModal =()=> setModalIsOpen(false);
        setModal();
    }, [props])


    const [modalIsOpen, setModalIsOpen] = useState(false);
    return (
        <div>
            <button onClick={() => setModalIsOpen(true)}>Modal Open</button>
            <Modal isOpen={modalIsOpen} onRequestClose={() => setModalIsOpen(false)} className="Modal" >
                <div className="modalBody">
                    <div className='recode containor'>
                        <a>Recode</a>
                        <input
                            type="text"
                            value="time"
                            disabled />
                    </div>
                    <div className='Nickname containor'>
                        <a>Nickname</a>
                        <input
                            type="text"
                            placeholder='닉네임을 입력해주세요'
                            value={nameInput}
                            onChange={({ target: { value } }) => setNameInput(value)}
                        />
                    </div>
                    <button className='regist-btn' onClick={updateName} > 등록 </button>
                </div>
            </Modal>
        </div>

    )
}

export default InsertName;