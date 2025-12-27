import React from 'react';
import ReactDOM from 'react-dom/client';
import App from './App';
import Error from './Error';
import Add from './Add';
import { BrowserRouter, Route, Routes, useParams } from 'react-router-dom';

const DayWrapper = () => {
  const { nb } = useParams();
  const day = nb ? parseInt(nb, 10) : 0;

  return (<App day={day}/>);
};

const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
  <BrowserRouter>
    <Routes>
      <Route path="/" element={<App day={0}/>} />
      <Route path="/day/:nb" element={<DayWrapper />} />
      <Route path="/add" element={<Add />} />
      <Route path="*" element={<Error/>} />
    </Routes>
  </BrowserRouter>
);