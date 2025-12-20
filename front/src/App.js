import React from 'react';
import './App.css';
import { useState, useEffect } from 'react';

export default function App() {
    const [data, setData] = useState(null);

    useEffect(() => {
        async function fetchData() {
            try {
                const response = await fetch('http://localhost:8888/');
                const json = await response.json();
                setData(json);
            } catch (error) {
                console.error("Erreur lors de la récupération des données :", error);
            }
        }

        fetchData();
    }, []);

    // Si le json n'a toujours pas ete traitee
    if (!data) {
        return (<div>Chargement des données...</div>);
    }

    // Si il n'y a pas de lecons a faire
    if (data.length === 0) {
        return (<div>Aucune leçons a réviser</div>);
    }

    // Sinon
    return (
        <ul>
            {data.map((item, i) => {
                return <li key={i}>
                    <strong>LEÇON :</strong> {item.name} 
                    <br />
                    Matière : {item.subject}
                    <br />
                    Numéro : {item.number}
                    <br />
                    Date : {item.date}
                </li>
            })}
        </ul>
    );
}

