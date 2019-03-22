// Auto-generated. Do not edit.
//% color=#E3008C weight=96 icon="\uf012"
declare namespace magneto {
    enum Direction {
        Left = 0,
        Right = 1,
        Up = = 2,
        Down = 3
    }
    
    /**
     * Test function for Magneto
     */
    //% weight=50
    //% blockId=magneto_get_number block="Gets a number from magneto" blockGap=8
    //% shim=magneto::testFunction
    function testFunction(): number;
    
    /**
     * Set the Micro:bits name to be sent over detection
     */
    //% weight=50
    //% blockId=magneto_set_name block="Set Micro:bit name %name" blockGap=8
    //% shim=magneto::setMicrobitName
    function setMicrobitName(name : string): void;
    
    /**
     * Device detected
     */
    //% weight=50
    //% blockId=magneto_device_detected_event block="Micro:bit Detected" blockGap=8
    //% shim=magneto::onDeviceDetected
    function onDeviceDetected(body: () => void): void;

    /**
     * Device detected in specific direction
     */
    //% weight=50
    //% blockId=magneto_device_detected_direction_event block="Micro:bit Detected To My %direction" blockGap=8
    //% shim=magneto::onDeviceInDirection
    function onDeviceDetectedDirection(direction : Direction, body: () => void): void;
    
    /**
     * Device detected with name detected
     */
    //% weight=50
    //% blockId=magneto_device_detected_name_event block="Micro:bit %name Detected" blockGap=8
    //% shim=magneto::onDeviceDetectedName
    function onDeviceDetectedName(name : string, body: () => void): void;
    
    /**
     * Device detected with name in a specific direction
     */
    //% weight=50
    //% blockId=magneto_device_detected_name_direction_event block="Micro:bit %name Detected To My %direction" blockGap=8
    //% shim=magneto::onDeviceDetectedNameDirection
    function onDeviceDetectedNameDirection(name : string, direction : Direction, body: () => void): void;
} // Auto-generated. Do not edit. Really.
