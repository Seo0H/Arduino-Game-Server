import React from "react";
import Modal from 'react-modal';
import { useState } from 'react';
import {Link} from 'react-router-dom'
import { useTable, useGlobalFilter, useSortBy } from "react-table";
import ".//css/styles.css";
import ".//css/modal.css";
import Search from "./Search";
import Table from "./Table";

function Side_Headers({ columns, data }) {

    const {
        getTableProps,
        getTableBodyProps,
        headerGroups,
        rows,
        prepareRow,
        setGlobalFilter
    } = useTable({ columns, data }, useGlobalFilter, useSortBy);


    const [modalIsOpen, setModalIsOpen] = useState(false);

    return (
        <div className="side-header-containor">
            <div className="title-containor">
                <div className="title">
                    <h1>Arduino</h1>
                    <h1>Game </h1>
                </div>
                <a className="title-detail">Made by SeoYoungJoo</a>
            </div>
            <div className="round-button">
                {/* <button
                    className="btn score"
                    onClick={() => setModalIsOpen(true)}>
                    Search
                </button> */}
                <a href ="https://github.com/dudwn1814/Arduino_socket">
                <button className="btn about">About Game</button>
                </a>
            </div>
            <Modal
                isOpen={modalIsOpen}
                onRequestClose={() => setModalIsOpen(false)}
                className="Modal-search" >
                <Search
                    onSubmit={setGlobalFilter}
                />
                <Table
                    columns={columns}
                    data={data}
                    className="table"
                />
            </Modal>
        </div>
    )
}

export default Side_Headers;