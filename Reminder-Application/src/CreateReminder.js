import { StyleSheet, Text, Button, TextInput, Alert } from 'react-native'
import React, { useState, useEffect } from 'react'
import { SafeAreaView } from "react-native-safe-area-context";
import { app } from "../firebase/firebaseConfig"
import { getFirestore, collection, addDoc, query, where, data, setDoc, doc } from "firebase/firestore";
import { getAuth } from "firebase/auth"

const auth = getAuth(app);
const db = getFirestore(app);

export default function CreateReminder({ navigation }) {

    // initialising state variables
    const [uid, setUid] = useState("");
    const [medicineName, setMedicineName] = useState("");
    const [time, setTime] = useState("");

    // loads uid upon page load
    useEffect(() => {
        const user = auth.currentUser
        if (user) setUid(user.uid);
    })

    // adds a reminder to the database
    const addReminder = async (userId, medicineName, time) => {
        try {
            // Reference to user's document inside "reminders" collection
            const userDocRef = doc(db, "reminders", userId);

            await setDoc(userDocRef, { medicineName, time }, { merge: true });

            Alert.alert("Success", "Reminder added successfully!");
            navigation.navigate("Navigate");
        } catch (error) {
            console.error("Error adding reminder:", error);
            Alert.alert("Error", "Failed to add reminder.");
        }
    };

    return (
        <SafeAreaView style={styles.container}>
            {/* Setti ng up a reminder */}
            <Text style={styles.header}>Add a Reminder</Text>
            <TextInput
                placeholder="Medicine Name"
                value={medicineName}
                onChangeText={setMedicineName}
                style={styles.textInput}
            />
            <TextInput
                placeholder="Time (YYYY-MM-DDTHH:mm:ssZ)"
                value={time}
                onChangeText={setTime}
                style={styles.textInput}
            />
            <Button title="Add Reminder" style={styles.button} onPress={() => { addReminder(uid, medicineName, time) }} />
            <Button title="Back" onPress={() => { navigation.navigate("Navigation") }} />


        </SafeAreaView>
    )
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        alignItems: "center",
        justifyContent: "center",
        gap: "3%"
    },
    header: {
        fontSize: 30,
        fontWeight: "bold"
    },
    textInput: {
        borderWidth: 1,
        padding: 8,
        marginVertical: 10,
        width: "90%"
    },
    button: {
        backgroundColor: "yellow"

    }


})