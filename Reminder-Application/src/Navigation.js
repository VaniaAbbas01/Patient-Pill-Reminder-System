import { StyleSheet, Text, TouchableOpacity, Button } from 'react-native'
import React, { useState, useEffect } from 'react'
import { SafeAreaView } from "react-native-safe-area-context";
import { app } from "../firebase/firebaseConfig"
import { getAuth, signOut } from "firebase/auth";

// Initialize Firebase
const auth = getAuth(app);

export default function Navigation({ navigation }) {

    // decalaring state variable
    const [uid, setUid] = useState(null);

    // fetched uid upon page load
    useEffect(() => {
        const user = auth.currentUser
        if (user) setUid(user.uid);
    })

    // signs user out
    const handleLogout = async () => {
        await signOut(auth);
    };

    return (
        <SafeAreaView style={styles.container}>
            {/* page header */}
            <Text style={styles.title}>Pill Reminder System</Text>
            {/* Authentication button if the user is not logged in */}
            {
                !uid ?
                    <TouchableOpacity style={styles.buttons} onPress={() => { navigation.navigate("Authentication") }}>
                        <Text style={styles.buttonText}>Authenticate</Text>
                    </TouchableOpacity> :
                    <></>
            }
            {/* Create a Reminder  button */}
            <TouchableOpacity style={styles.buttons} onPress={() => { navigation.navigate("CreateReminder") }}>
                <Text style={styles.buttonText}>Create A Reminder</Text>
            </TouchableOpacity>
            {/* View Health Data Button */}
            <TouchableOpacity style={styles.buttons} onPress={() => { navigation.navigate("ViewHealthData") }}>
                <Text style={styles.buttonText}>View Health Data</Text>
            </TouchableOpacity>
            {/* logout button if the user is logged in */}
            {
                uid ?
                    <Button title="Logout" onPress={handleLogout} />
                    :
                    <></>
            }
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
    title: {
        fontSize: 30,
        fontWeight: "bold"
    },
    buttons: {
        backgroundColor: "yellow",
        width: "90%",
        height: "10%"
    },
    buttonText: {
        textAlign: "center",
        paddingTop: "8%",
        fontSize: 15

    }
})