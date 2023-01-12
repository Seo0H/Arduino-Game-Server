import React from "react";
import ".//css/modal.css"

function Search({ onSubmit }) {
  const handleSubmit = (event) => {
    event.preventDefault();
    onSubmit(event.target.elements.filter.value);
  };

  return (
    <form onSubmit={handleSubmit}>
      <input className="input-box" name="filter" />
      <button className="serch-btn">Search</button>
    </form>
  );
}

export default Search;
